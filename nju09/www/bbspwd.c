#include "bbslib.h"


// ����һ����Ϊlen������ַ���
char *random_str(char *buf, int len)
{
	srand((unsigned)time(NULL));
	int i;
	for (i = 0; i < len; i++)
		buf[i] = rand()%10 + '0';
	buf[len] = '\0';
	return buf;
}

int
bbspwd_main() {
	//modify by mintbaggio 20040829 for new www
	int type;
	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼, ���ȵ�¼");
	changemode(GMENU);

	int mode = atoi(getparm("mode"));

	//�޸ı�������
	if (mode == 1)
	{
		char pw1[20], pw2[20], pw3[20], salt[3];
		printf("<body>");
		type = atoi(getparm("type"));
		if (type == 0) {
			printf("<div class=rhead>%s -- �޸����� [�û�: <span class=h11>%s</span>]</div><hr>\n", BBSNAME, currentuser.userid);
			printf("<form action=bbspwd?mode=1&type=1 method=post>\n");
			printf("��ľ�����: <input maxlength=12 size=12 type=password name=pw1><br>\n");
			printf("���������: <input maxlength=12 size=12 type=password name=pw2><br>\n");
			printf("������һ��: <input maxlength=12 size=12 type=password name=pw3><br><br>\n");
			printf("<input type=submit value=ȷ���޸�>\n");
			printf("</body>");
			http_quit();
		}
		ytht_strsncpy(pw1, getparm("pw1"), 13);
		ytht_strsncpy(pw2, getparm("pw2"), 13);
		ytht_strsncpy(pw3, getparm("pw3"), 13);
		if (strcmp(pw2, pw3))
			http_fatal("�������벻��ͬ");
		if (strlen(pw2) < 2)
			http_fatal("������̫��");
		if (!ytht_crypt_checkpasswd(currentuser.passwd, pw1))
			http_fatal("���벻��ȷ");
		getsalt(salt);
		strcpy(currentuser.passwd, ytht_crypt_crypt1(pw2, salt));
		save_user_data(&currentuser);
		printf("[%s] �����޸ĳɹ�.", currentuser.userid);
	}

	return 0;
}
