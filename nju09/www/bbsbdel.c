#include "bbslib.h"

int
bbsbdel_main()
{
	FILE *fp;
	int i, total = 0;
	char path[80], userid[80];
	struct override b[MAXREJECTS];
	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼�����ȵ�¼");
	changemode(GMENU);
	sethomefile(path, currentuser.userid, "rejects");
	printf("<center>%s -- ������ [ʹ����: %s]<hr>\n", BBSNAME,
	       currentuser.userid);
	strsncpy(userid, getparm("userid"), 13);
	if (userid[0] == 0) {
		printf("<form action=bbsbdel>\n");
		printf("��������ɾ���ĺ������ʺ�: <input type=text><br>\n");
		printf("<input type=submit>\n");
		printf("</form>");
		http_quit();
	}
	loadbad(currentuser.userid);
	if (badnum <= 0)
		http_fatal("��û���趨�κκ�����");
	if (!isbad(userid))
		http_fatal("���˱����Ͳ�����ĺ�������");
	for (i = 0; i < badnum; i++) {
		if (strcasecmp(bbb[i].id, userid)) {
			memcpy(&b[total], &bbb[i], sizeof (struct override));
			total++;
		}
	}
	fp = fopen(path, "w");
	fwrite(b, sizeof (struct override), total, fp);
	fclose(fp);
	printf
	    ("[%s]�Ѵ����ĺ�������ɾ��.<br>\n <a href=bbsball>���غ�����</a>",
	     userid);
	http_quit();
	return 0;
}
