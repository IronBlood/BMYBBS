#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "ytht/shmop.h"
#include "ythtbbs/cache.h"
#include "ythtbbs/record.h"
#include "ythtbbs/user.h"
#include "cache-internal.h"
#include "ythtbbs/override.h"

/***** global variables *****/
static struct ythtbbs_cache_UserIDHashTable *shm_userid_hashtable;
static struct ythtbbs_cache_UserTable       *shm_user_table;

/***** prototypes of private functions *****/
static int ythtbbs_cache_UserTable_fill_v(void *user_ec, va_list ap);
static int ythtbbs_cache_UserIDHashTable_resolve();
static int ythtbbs_cache_UserIDHashTable_insert(char *userid, int idx);

/***** implementations of public functions *****/
unsigned int ythtbbs_cache_User_hash(const char *userid) {
	unsigned int n1 = 0;
	unsigned int n2 = 0;
	const unsigned int _HASH_SIZE = 26;

	while (*userid) {
		n1 += ((unsigned int) toupper(*userid)) % _HASH_SIZE;
		userid++;
		if (!*userid) break;

		n2 += ((unsigned int ) toupper(*userid)) % _HASH_SIZE;
		userid++;
	}

	n1 %= _HASH_SIZE;
	n2 %= _HASH_SIZE;
	return n1 * _HASH_SIZE + n2;
}

void ythtbbs_cache_UserTable_resolve() {
	int fd;
	struct stat st;
	static volatile time_t lasttime = -1;
	time_t local_now;
	char   local_buf[100];
	int    local_usernumber;

	fd = open(MY_BBS_HOME "/.CACHE.user.lock", O_RDWR | O_CREAT, 0660);
	flock(fd, LOCK_EX);

	time(&local_now);
/*
	if (local_now - lasttime < 10) {
		// do not update if already updated in the last 10 seconds
		close(fd);
		return;
	}
*/

	lasttime = local_now;

	if (shm_user_table == NULL) {
		shm_user_table = get_shm(UCACHE_SHMKEY, sizeof(*shm_user_table));
		if (shm_user_table == NULL)
			shm_err(UCACHE_SHMKEY);
	}

	if (stat(FLUSH, &st) < 0) {
		st.st_mtime++;
	}

	if (shm_user_table->update_time < st.st_mtime) {
		local_usernumber = 0;
		ythtbbs_record_apply_v(PASSFILE, ythtbbs_cache_UserTable_fill_v, sizeof(struct userec), &local_usernumber);

		shm_user_table->number = local_usernumber;
		shm_user_table->update_time = st.st_mtime;
		shm_user_table->usersum = 0; // TODO;

		sprintf(local_buf, "system reload ucache %d", shm_user_table->usersum);
		// TODO detach shm and reattach?
	}
	close(fd);

	ythtbbs_cache_UserIDHashTable_resolve();
}

void ythtbbs_cache_UserTable_add_utmp_idx(int uid, int utmp_idx) {
	int i, idx;

	if (uid <= 0 || uid > MAXUSERS)
		return;

	// 检查是否已存在
	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		if (shm_user_table->users[uid - 1].utmp_indices[i] == utmp_idx + 1)
			return;
	}

	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		idx = shm_user_table->users[uid - 1].utmp_indices[i] - 1;
		if (idx < 0 || !ythtbbs_cache_utmp_check_active_by_idx(idx) || !ythtbbs_cache_utmp_check_uid_by_idx(idx, uid)) {
			// TODO check
			shm_user_table->users[uid - 1].utmp_indices[i] = utmp_idx + 1;
			return;
		}
	}
}

void ythtbbs_cache_UserTable_remove_utmp_idx(int uid, int utmp_idx) {
	int i;

	if (uid <= 0 || uid > MAXUSERS)
		return;

	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		if (shm_user_table->users[uid - 1].utmp_indices[i] == utmp_idx + 1) {
			shm_user_table->users[uid - 1].utmp_indices[i] = 0;
			return;
		}
	}
}

int ythtbbs_cache_UserTable_searchnewuser() {
	register int num, i;

	ythtbbs_cache_UserTable_resolve();
	num = shm_user_table->number;

	for (i = 0; i < num; i++) {
		if (shm_user_table->users[i].userid[0] == '\0')
			return i + 1;
	}

	if (num < MAXUSERS)
		return (num + 1);

	return 0;
}

void ythtbbs_cache_UserTable_setuserid(int usernum, char *userid) {
	int user_idx;

	if (usernum > 0 && usernum <= MAXUSERS) {
		if (usernum > shm_user_table->number)
			shm_user_table->number = usernum;

		user_idx = usernum - 1;
		memset(&shm_user_table->users[user_idx], 0, sizeof(struct ythtbbs_cache_User));
		strncpy(shm_user_table->users[user_idx].userid, userid, IDLEN+1);
		shm_user_table->users[user_idx].userid[IDLEN] = '\0';
	}
}

void ythtbbs_cache_UserTable_getuserid(int usernum, char *userid, size_t len) {
	ythtbbs_cache_UserTable_resolve();

	if (usernum > 0 && usernum <= MAXUSERS && usernum <= shm_user_table->number) {
		strncpy(userid, shm_user_table->users[usernum - 1].userid, len);
		shm_user_table->users[usernum - 1].userid[len - 1] = '\0';
	} else {
		userid[0] = '\0';
	}
}

char *ythtbbs_cache_UserTable_get_namearray(char buf[][IDLEN + 1], int *pnum, char *tag, int (*fptr)(char *, char *, char *)) {
	register struct ythtbbs_cache_UserTable *reg_user_table;
	register char *ptr, tmp;
	register int n, total;
	char tagbuf[STRLEN];
	int ch, num = 0;

	ythtbbs_cache_UserIDHashTable_resolve();
	reg_user_table = shm_user_table;

	if (tag[0] == '\0') {
		*pnum = reg_user_table->number;
		total = reg_user_table->number;
		for (n = 0; n < total;) {
			ptr = reg_user_table->users[n].userid;
			if (ptr[0] == '\0')
				continue;

			strcpy(buf[num++], ptr);
			n++;
		}
		return buf[0];
	}

	for (n = 0; tag[n] != '\0'; n++) {
		tagbuf[n] = toupper(tag[n]);
	}
	tagbuf[n] = '\0';
	ch = tagbuf[0];

	total = reg_user_table->number;
	for (n = 0; n < total; n++) {
		ptr = reg_user_table->users[n].userid;
		tmp = *ptr;
		if (tmp == ch || tmp == ch - 'A' + 'a')
			if (fptr(tag, tagbuf, ptr))
				strcpy(buf[num++], ptr);
	}
	*pnum = num;
	return buf[0];
}

/**
 * @brief 将 UserTable 缓存中的用户信息序列化出来
 * 输出形式 user_idx, userid [ session ]："0, SYSOP [ [0,42] ]\n"
 * @warning 非公开接口
 */
void ythtbbs_cache_UserTable_dump(FILE *fp) {
	int i; // MAXUSERS
	int j;
	struct ythtbbs_cache_User *user;

	ythtbbs_cache_UserTable_resolve();
	fprintf(fp, "===== UserTable =====\n");
	for (i = 0; i < MAXUSERS; i++) {
		user = &(shm_user_table->users[i]);
		if (user->userid[0] == '\0')
			continue;

		fprintf(fp, "%d, %s [ ", i, user->userid);
		for (j = 0; j < MAX_LOGIN_PER_USER; j++) {
			if (user->utmp_indices[j] == 0)
				continue;

			fprintf(fp, "[%d,%d] ", j, user->utmp_indices[j]);
		}

		fprintf(fp, "]\n");
	}
}

bool ythtbbs_cache_UserTable_is_friend_online_by_uid(const char *userid, bool has_see_cloak_perm, unsigned search_uid) {
	int i, utmp_idx, testreject = 0;
	struct user_info *ptr_info;

	if (search_uid <= 0 || search_uid > MAXUSERS)
		return false;

	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		utmp_idx = shm_user_table->users[search_uid - 1].utmp_indices[i] - 1;

		if (utmp_idx < 0) // starts from 0
			continue;

		ptr_info = ythtbbs_cache_utmp_get_by_idx(utmp_idx);
		if (!ptr_info->active || !ptr_info->pid || ptr_info->uid != search_uid)
			continue;

		if (!testreject) {
			if (ythtbbs_override_included(userid, YTHTBBS_OVERRIDE_REJECTS, ptr_info->userid))
				return true;

			testreject = 1;
		}

		if (ptr_info->invisible && !has_see_cloak_perm)
			continue;

		return true;
	}

	return false;
}

/**
 * @brief 将 UserIDHashTable 缓存中的用户信息序列化出来
 * 输出形式为 hashid, user_num, userid："42, 0, SYSOP\n"
 * @warning 非公开接口
 */
void ythtbbs_cache_UserIDHashTable_dump(FILE *fp) {
	int i; // UCACHE_HASH_SIZE
	struct ythtbbs_cache_UserIDHashItem *item;

	ythtbbs_cache_UserIDHashTable_resolve();
	fprintf(fp, "===== UserIDHashTable =====\n");
	for (i = 0; i < UCACHE_HASH_SIZE; i++) {
		item = &(shm_userid_hashtable->items[i]);
		if (item->userid[0] == '\0')
			continue;

		fprintf(fp, "%d, %d, %s\n", i, item->user_num, item->userid);
	}
}

int ythtbbs_cache_UserTable_get_user_online_friends(const char *userid, bool has_see_cloak_perm, struct user_info *user_list, size_t user_list_size) {
	int lockfd;
	unsigned int i, j, k, total, user_idx;
	struct ythtbbs_override *override_friends = NULL;
	const struct user_info *x;

	lockfd = ythtbbs_override_lock(userid, YTHTBBS_OVERRIDE_FRIENDS);
	total = ythtbbs_override_count(userid, YTHTBBS_OVERRIDE_FRIENDS);

	if (total <= 0) {
		ythtbbs_override_unlock(lockfd);
		return 0;
	}

	override_friends = (struct ythtbbs_override*) calloc(total, sizeof(struct ythtbbs_override));
	ythtbbs_override_get_records(userid, override_friends, total, YTHTBBS_OVERRIDE_FRIENDS);
	ythtbbs_override_unlock(lockfd);

	ythtbbs_cache_UserTable_resolve();
	for (i = 0, k = 0; i < total; i++) {
		user_idx = ythtbbs_cache_UserIDHashTable_find_idx(override_friends[i].id);
		if (user_idx < 0)
			continue;

		for (j = 0; j < MAX_LOGIN_PER_USER; j++) {
			if (shm_user_table->users[user_idx].utmp_indices[j] > 0) {
				x = ythtbbs_cache_utmp_get_by_idx(shm_user_table->users[user_idx].utmp_indices[j] - 1);
				if (x->invisible && !has_see_cloak_perm)
					continue;

				memcpy(&user_list[k], x, sizeof(struct user_info));
				k++;
				if (k == user_list_size)
					goto END;
			}
		}
	}

END:
	if (override_friends)
		free(override_friends);
	return k;
}

bool ythtbbs_cache_UserTable_is_user_online(const char *userid) {
	int user_idx, i;
	struct ythtbbs_cache_User *user;
	struct user_info *info;
	time_t t;

	user_idx = ythtbbs_cache_UserIDHashTable_find_idx(userid);
	if (user_idx < 0)
		return false;

	user = &shm_user_table->users[user_idx];
	time(&t);
	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		if (user->utmp_indices[i] == 0)
			continue;

		info = ythtbbs_cache_utmp_get_by_idx(user->utmp_indices[i] - 1);
		if (info->active == 0)
			continue;

		if (strcmp(info->userid, user->userid) == 0) {
			if (info->pid > 1) {
				// telnet 或者 ssh 方式
				return true;
			} else {
				// NJU09 或者 API 方式，认定为 5 分钟内
				if (t - info->lasttime < 5 * 60) {
					return true;
				}
			}
		}
	}
	return false;
}

bool ythtbbs_cache_UserTable_is_user_invisible(const char *userid) {
	int user_idx, i;
	struct ythtbbs_cache_User *user;
	struct user_info *info;

	user_idx = ythtbbs_cache_UserIDHashTable_find_idx(userid);
	if (user_idx < 0)
		return false;

	user = &shm_user_table->users[user_idx];
	for (i = 0; i < MAX_LOGIN_PER_USER; i++) {
		if (user->utmp_indices[i] == 0)
			continue;

		info = ythtbbs_cache_utmp_get_by_idx(user->utmp_indices[i] - 1);
		if (info->active == 0)
			continue;

		if (strcmp(info->userid, user->userid) == 0) {
			if (info->invisible)
				return true;
		}
	}
	return false;
}

/***** implementations of private functions *****/
static int ythtbbs_cache_UserTable_fill_v(void *user_ec, va_list ap) {
	int           *ptr_local_usernumber;
	struct userec *ptr;

	ptr_local_usernumber = va_arg(ap, int *);
	ptr = (struct userec *)user_ec;
	if (*ptr_local_usernumber < MAXUSERS) {
		if (strcmp(shm_user_table->users[*ptr_local_usernumber].userid, ptr->userid) != 0) {
			// shm_user_table->users[i].userid 只可能存在两种情况
			// 1) userid 全部为 '\0' 的初始状态
			// 2) userid 已被设置且使用 '\0' 作为终止符
			// 因此这里使用 strcmp 应该是安全的，并且只在 userid 不相同的时候填充并初始化 utmp_indices
			strncpy(shm_user_table->users[*ptr_local_usernumber].userid, ptr->userid, IDLEN+1);
			shm_user_table->users[*ptr_local_usernumber].userid[IDLEN] = '\0';
			memset(shm_user_table->users[*ptr_local_usernumber].utmp_indices, 0, sizeof(int) * MAX_LOGIN_PER_USER);
		}
		(*ptr_local_usernumber)++;
	}
	return 0;
}

static int ythtbbs_cache_UserIDHashTable_resolve() {
	int fd, i;
	char local_buf[64];

	fd = open(MY_BBS_HOME "/.CACHE.hash.lock", O_RDWR | O_CREAT, 0660);
	flock(fd, LOCK_EX);
	if (shm_userid_hashtable == NULL) {
		shm_userid_hashtable = get_shm(UCACHE_HASH_SHMKEY, sizeof(*shm_userid_hashtable));
		if (shm_userid_hashtable == NULL) {
			shm_err(UCACHE_HASH_SHMKEY);
		}
	}

	if (shm_userid_hashtable->update_time < shm_user_table->update_time) {
		shm_userid_hashtable->update_time = shm_user_table->update_time;

		for(i = 0; i < shm_user_table->number; i++) {
			ythtbbs_cache_UserIDHashTable_insert(shm_user_table->users[i].userid, i);
		}

		sprintf(local_buf, "system reload shm_userid_hashtable %d", shm_user_table->number);
		newtrace(local_buf);

		// TODO detach and reattach?
	}

	close(fd);
	return 0;
}

static int ythtbbs_cache_UserIDHashTable_insert(char *userid, int idx) {
	unsigned int h, s, i, j = 0;
	int old_idx;
	char local_buf[128];
	struct ythtbbs_cache_UserIDHashItem *ptr_items = shm_userid_hashtable->items;
	if (!*userid)
		return -1;

	old_idx = ythtbbs_cache_UserIDHashTable_find_idx(userid);
	if (old_idx >= 0) {
		// 记录已存在于 UserIDHashTable 中，则不处理
		if (old_idx != idx) {
			// 理应相等
			snprintf(local_buf, sizeof(local_buf), "user_idx changed? %d --> %d", old_idx, idx);
			newtrace(local_buf);
		}

		return 0;
	}
	h = ythtbbs_cache_User_hash(userid);
	s = UCACHE_HASH_SIZE / 26 / 26;
	i = h * s;

	// 找到第一个可用的位置，最多跨越 5 块区域
	while ((j < s * 5) && ptr_items[i].user_num > 0 && ptr_items[i].user_num != idx + 1) {
		j++;
		i++;
		if (i >= UCACHE_HASH_SIZE)
			i %= UCACHE_HASH_SIZE;
	}

	if (j == s * 5) {
		// 如果跨越 5 块区域都没有找到可用的位置，则 hash 失败
		return -1;
	}

	ptr_items[i].user_num = idx + 1;
	strcpy(ptr_items[i].userid, userid);
	return 0;
}

int ythtbbs_cache_UserIDHashTable_find_idx(const char *userid) {
	unsigned int h, s, i, j;
	struct ythtbbs_cache_UserIDHashItem *ptr_items = shm_userid_hashtable->items;
	if (!*userid)
		return -1;

	h = ythtbbs_cache_User_hash(userid);
	s = UCACHE_HASH_SIZE / 26 / 26;
	i = h * s;

	for (j = 0; j < s * 5; j++) {
		if (!strcasecmp(ptr_items[i].userid, userid))
			return ptr_items[i].user_num - 1;

		i++;
		if (i >= UCACHE_HASH_SIZE)
			i %= UCACHE_HASH_SIZE;
	}

	return -1;
}

