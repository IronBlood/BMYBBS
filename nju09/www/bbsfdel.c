#include "bbslib.h"

int
bbsfdel_main()
{
	unsigned int i;
	int lockfd;
	char userid[80];
	struct ythtbbs_override f[MAXFRIENDS], EMPTY;
	struct userec *lookupuser;

	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼�����ȵ�¼");
	changemode(GMENU);
	printf("<center>%s -- �������� [ʹ����: %s]<hr>\n", BBSNAME, currentuser.userid);
	ytht_strsncpy(userid, getparm("userid"), 13);
	if (userid[0] == 0) {
		printf("<form action=bbsfdel>\n");
		printf("��������ɾ���ĺ����ʺ�: <input type=text><br>\n");
		printf("<input type=submit>\n");
		printf("</form>");
		http_quit();
	}

	lockfd = ythtbbs_override_lock(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS);
	friendnum = ythtbbs_override_count(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS);
	if (friendnum <= 0) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("��û���趨�κκ���");
	}

	lookupuser = getuser(userid);
	if (!lookupuser || !ythtbbs_override_included(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS, lookupuser->userid)) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("���˱����Ͳ�����ĺ���������");
	}

	ythtbbs_override_get_records(currentuser.userid, f, friendnum, YTHTBBS_OVERRIDE_FRIENDS);
	for (i = 0; i < friendnum; i++) {
		f[i].id[sizeof(EMPTY.id) - 1] = 0;
		if (strcasecmp(f[i].id, userid) == 0) {
			f[i].id[0] = '\0';
			friendnum--;
			break; // found
		}
	}
	ythtbbs_override_set_records(currentuser.userid, f, friendnum, YTHTBBS_OVERRIDE_FRIENDS);
	ythtbbs_override_unlock(lockfd);

	printf("[%s]�Ѵ����ĺ���������ɾ��.<br>\n <a href=bbsfall>���غ�������</a>", userid);
	http_quit();
	return 0;
}

