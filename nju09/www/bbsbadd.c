#include "bbslib.h"

int
bbsbadd_main()
{
	FILE *fp;
	char path[80], userid[80], exp[80];
	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼�����ȵ�¼");
	changemode(GMENU);
	sethomefile(path, currentuser.userid, "rejects");
	printf("<body><center>%s -- ������ [ʹ����: %s]<hr>\n", BBSNAME,
	       currentuser.userid);
	strsncpy(userid, getparm("userid"), 13);
	strsncpy(exp, getparm("exp"), 32);
	loadbad(currentuser.userid);
	if (userid[0] == 0 || exp[0] == 0) {
		if (userid[0])
			printf("<font color=red>�����������˵��</font>");
		printf("<form action=bbsbadd>\n");
		printf
		    ("������������ĺ������ʺ�: <input type=text name=userid value='%s'><br>\n",
		     userid);
		printf
		    ("�����������������˺ŵ�˵��: <input type=text name=exp value='%s'>\n",
		     exp);
		printf("<br><input type=submit value=ȷ��></form>\n");
		http_quit();
	}
	if (!getuser(userid))
		http_fatal("�����ʹ�����ʺ�");
	if (badnum >= MAXREJECTS-1)
		http_fatal("���ĺ������Ѵﵽ����, ��������µĺ�����");
	if (isfriend(userid))
		http_fatal("�����Ѿ�����ĺ���������");
	strcpy(bbb[badnum].id, getuser(userid)->userid);
	strcpy(bbb[badnum].exp, exp);
	badnum++;
	fp = fopen(path, "w");
	fwrite(bbb, sizeof (struct override), badnum, fp);
	fclose(fp);
	printf("[%s]�Ѽ������ĺ�����.<br>\n <a href=bbsball>���غ�����</a>",
	       userid);
	http_quit();
	return 0;
}
