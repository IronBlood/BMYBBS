#include "bbslib.h"

int
bbsball_main()
{	//modify by mintbaggio 20040829 for new www
	int i;
	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼, ���ȵ�¼");
	changemode(GMENU);
	loadbad(currentuser.userid);
	printf("<body><center>\n");
	printf("<div class=rhead>%s -- ������ [ʹ����: <span class=h11>%s</span>]</div><hr><br>\n", BBSNAME,
	       currentuser.userid);
	printf("�����趨�� %d λ������<br>", badnum);
	printf
	    ("<table border=1><tr><td>���</td><td>����������</td><td>������˵��</td><td>�Ӻ�����ɾ��</td></tr>");
	for (i = 0; i < badnum; i++) {
		printf("<tr><td>%d</td>", i + 1);
		printf("<td><a href=bbsqry?userid=%s>%s</a></td>", bbb[i].id,
		       bbb[i].id);
		printf("<td>%s</td>\n", nohtml(bbb[i].exp));
		printf
		    ("<td>[<a onclick='return confirm(\"ȷʵɾ����?\")' href=bbsbdel?userid=%s>ɾ��</a>]</td></tr>",
		     bbb[i].id);
	}
	printf("</table><hr>\n");
	printf("[<a href=bbsbadd>����µĺ�����</a>]</center></body>\n");
	http_quit();
	return 0;
}
