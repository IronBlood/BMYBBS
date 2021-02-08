#include "bbslib.h"

int
bbsfadd_main()
{
	struct userec *lookupuser;
	int lockfd;
	struct ythtbbs_override of;

	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼�����ȵ�¼");
	changemode(GMENU);
	printf("<body><center>%s -- �������� [ʹ����: %s]<hr>\n", BBSNAME, currentuser.userid);
	ytht_strsncpy(of.id, getparm("userid"), 13);
	ytht_strsncpy(of.exp, getparm("exp"), 32);
	if (of.id[0] == 0 || of.exp[0] == 0) {
		if (of.exp[0])
			printf("<font color=red>���������˵��</font>");
		printf("<form action=bbsfadd>\n");
		printf("������������ĺ����ʺ�: <input type=text name=userid value='%s'><br>\n", (of.id[0] == 0) ? "" : of.id);
		printf("�������������ѵ�˵��: <input type=text name=exp value='%s'>\n", (of.exp[0] == 0) ? "" : of.exp);
		printf("<br><input type=submit value=ȷ��></form>\n");
		http_quit();
	}

	lookupuser = getuser(of.id);
	if (!lookupuser)
		http_fatal("�����ʹ�����ʺ�");

	lockfd = ythtbbs_override_lock(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS);
	friendnum = ythtbbs_override_count(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS);
	if (friendnum >= MAXFRIENDS-1) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("���ĺ��������Ѵﵽ����, ��������µĺ���");
	}

	if (ythtbbs_override_included(currentuser.userid, YTHTBBS_OVERRIDE_FRIENDS, lookupuser->userid)) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("�����Ѿ�����ĺ�����������");
	}

	ytht_strsncpy(of.id, lookupuser->userid, sizeof(of.id));
	ythtbbs_override_add(currentuser.userid, &of, YTHTBBS_OVERRIDE_FRIENDS);
	ythtbbs_override_unlock(lockfd);

	printf("[%s]�Ѽ������ĺ�������.<br>\n <a href=bbsfall>���غ�������</a>", of.id);
	http_quit();
	return 0;
}

