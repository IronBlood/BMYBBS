#include "bbslib.h"
#include "identify.h"

int
bbsreg_main()
{
	html_header(1);

	printf("<body>");
	printf("<style>code { background: #eee }</style>");
	printf("<nobr><center>%s -- ���û�ע��<hr>\n", BBSNAME);
	printf("<font color=green>��ӭ���뱾վ. ������������ʵ��д. ��*�ŵ���ĿΪ��������.</font>");
	printf("<br><font color=green>���� �� ID ��Ϊ���ڱ�վ��ʵ����ʾ���û����ƣ������޸ģ���������д.</font>");
	printf("<form method=post action=bbsdoreg>\n");
	printf("<table width=100%%>\n");
	printf("<tr><td align=right>*������ID  :<td align=left><input name=userid size=12 maxlength=12> (2-12�ַ�, ����ȫΪӢ����ĸ)\n");
	printf("<tr><td align=right>*����������:<td align=left><input type=password name=pass1 size=12 maxlength=12> (4-12�ַ�)\n");
	printf("<tr><td align=right>*��ȷ������:<td align=left><input type=password name=pass2 size=12 maxlength=12>\n");
	printf("<tr><td align=right>*�������ǳ�:<td align=left><input name=username size=20 maxlength=32> (2-30�ַ�, ��Ӣ�Ĳ���)\n");
	printf("<tr><td align=right>*������������ʵ����:<td align=left><input name=realname size=20> (��������, ����2������)\n");
	printf("<tr><td align=right>*��ϸͨѶ��ַ/Ŀǰסַ:<td align=left><input name=address size=40>  (����6���ַ�)\n");
	printf("<tr><td align=right>*ѧУϵ�����߹�˾��λ:<td align=left><input name=dept size=40>\n");

	printf("<tr><td align=right>����绰(��ѡ):<td align=left><input name=phone size=40>\n");
	printf("<tr><td align=right>У�ѻ���߱�ҵѧУ(��ѡ):<td align=left><input name=assoc size=40>\n");
	printf("<tr><td align=right>��վ����(��ѡ):<td align=left>");
	printf("<textarea name=words rows=3 cols=40 wrap=virutal></textarea>");

#ifdef POP_CHECK

	printf("<tr><td align=right>*�������ε��ʼ��������б�:<td align=left><SELECT NAME=popserver>\n");
	int n = 1;
	while(n <= DOMAIN_COUNT)
	{
		if (n == 1)
			printf("<OPTION VALUE=%s SELECTED>", MAIL_DOMAINS[n]);
		else
			printf("<OPTION VALUE=%s>", MAIL_DOMAINS[n]);

		printf("%s", MAIL_DOMAINS[n]);
		n++;

	}
	printf("</select>\n");
	printf("<tr><td align=right>*�����������û���������������test��:<td align=left><input name=user size=20 maxlength=20> \n");
	printf("<tr><td align=right><td align=left>ÿ��������������֤ %d ��bbs�ʺţ��˴��������� @ �ַ���ǰ�Ĳ��֣�������������Ϊ \"example@xjtu.edu.cn\"���˴����� \"example\" ���ɣ����������ţ���<br>������������б���ѡ��", MAX_USER_PER_RECORD);

#endif

	printf("</table><br><hr>\n");
	printf("<input type=submit value=�ύ���> <input type=reset value=������д> <input type=button value=�鿴���� onclick=\"javascript:{open('/reghelp.html','winreghelp','width=600,height=460,resizeable=yes,scrollbars=yes');return false;}\"\n");
	printf("</form></center>");
	http_quit();
	return 0;
}

