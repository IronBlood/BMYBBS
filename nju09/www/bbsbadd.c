#include "bbslib.h"

int
bbsbadd_main()
{
	struct userec *lookupuser;
	int lockfd;
	struct ythtbbs_override ob;

	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼�����ȵ�¼");
	changemode(GMENU);
	printf("<body><center>%s -- ������ [ʹ����: %s]<hr>\n", BBSNAME, currentuser.userid);
	ytht_strsncpy(ob.id, getparm("userid"), 13);
	ytht_strsncpy(ob.exp, getparm("exp"), 32);
	loadbad(currentuser.userid);
	if (ob.id[0] == 0 || ob.exp[0] == 0) {
		if (ob.id[0])
			printf("<font color=red>�����������˵��</font>");
		printf("<form action=bbsbadd>\n");
		printf("������������ĺ������ʺ�: <input type=text name=userid><br>\n");
		printf("�����������������˺ŵ�˵��: <input type=text name=exp>\n");
		printf("<br><input type=submit value=ȷ��></form>\n");
		http_quit();
	}

	lookupuser = getuser(ob.id);
	if (!lookupuser)
		http_fatal("�����ʹ�����ʺ�");

	lockfd = ythtbbs_override_lock(currentuser.userid, YTHTBBS_OVERRIDE_REJECTS);
	badnum = ythtbbs_override_count(currentuser.userid, YTHTBBS_OVERRIDE_REJECTS);
	if (badnum >= MAXREJECTS-1) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("���ĺ������Ѵﵽ����, ��������µĺ�����");
	}

	if (ythtbbs_override_included(currentuser.userid, YTHTBBS_OVERRIDE_REJECTS, lookupuser->userid)) {
		ythtbbs_override_unlock(lockfd);
		http_fatal("�����Ѿ�����ĺ���������");
	}

	strcpy(ob.id, lookupuser->userid);
	ythtbbs_override_add(currentuser.userid, &ob, YTHTBBS_OVERRIDE_REJECTS);
	ythtbbs_override_unlock(lockfd);

	printf("[%s]�Ѽ������ĺ�����.<br>\n <a href=bbsball>���غ�����</a>", ob.id);
	http_quit();
	return 0;
}
