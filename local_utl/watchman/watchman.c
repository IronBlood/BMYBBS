#include "bbs.h"
#include "ytht/random.h"

#define DEFAULT_ADD 600
#define CONT MY_BBS_HOME "/help/watchmanhelp"
void
usage(char *name)
{
	printf("%s userid userpasswd [minutes_to_delay]\n", name);
}

int
main(int argc, char *argv[])
{
	int time_add = 0;
	int i;
	FILE *fp, *fr;
	time_t now_t;
	char buf[256];
	if (argc < 3) {
		usage(argv[0]);
		return 0;
	}
	//errlog("argc:%d",argc);
	if (argc > 3){
		time_add = atoi(argv[3]) * 60;
	//	errlog("argv[3]: %s",argv[3]);
	}
	if (!time_add)
		time_add = DEFAULT_ADD;
	ythtbbs_cache_utmp_resolve();
	now_t = time(NULL);
	if (ythtbbs_cache_utmp_get_watchman()) {
		strcpy(buf, "�����ﾯ�������������԰�����ס�ˡ�����");
	} else {
		ythtbbs_cache_utmp_set_watchman(now_t + time_add);
		ythtbbs_cache_utmp_set_unlock();
		snprintf(buf, sizeof (buf), "������,��î��!������ %s", ytht_ctime(now_t));
	}

	fp = popen("/bin/mail bbs", "w");
	if (!fp) {
		errlog("can't open mail!");
		return -2;
	}
	fr = fopen(CONT, "r");
	fprintf(fp, "#name: %s\n", argv[1]);
	fprintf(fp, "#pass: %s\n", argv[2]);
	fprintf(fp, "#board: deleterequest\n");
	fprintf(fp, "#title: %s\n", buf);
	fprintf(fp, "#localpost:\n\n");
	if (!strncmp(buf, "��", 2)) {
		fprintf(fp,
			"����%d�����ڽ��н�������,����BBS���к�������صİ��潫�����\n"
			"ֱ�����˽����˽�������.\n" "��������: %u\n",
			time_add / 60, ythtbbs_cache_utmp_get_unlock() % 10000);
	}
	fprintf(fp, "�����ǹ��ڽ���������˵��:\n");
	fr = fopen(CONT, "r");
	if (fr) {
		while (fgets(buf, sizeof (buf), fr))
			fputs(buf, fp);
		fclose(fr);
	}
	fclose(fp);
	return 0;
}
