#include "bbslib.h"
#include "identify.h"

#ifdef POP_CHECK
// ��½�ʼ��������õ�ͷ�ļ� added by interma@BMY 2005.5.12
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// �ʼ����������û���������ĳ��ȣ� added by interma@BMY 2005.5.12
#define USER_LEN 20
#define PASS_LEN 20

#endif

int
bbsfindacc_main()
{
	size_t i;

	html_header(1);
	printf("<body>");

	char user[USER_LEN + 1];
    char pass[PASS_LEN + 1];
	char popserver[512];
	strsncpy(popserver, getparm("popserver1"), 512);
	strsncpy(user, getparm("user1"), USER_LEN);
	strsncpy(pass, getparm("pass1"), PASS_LEN);

	char delims[] = "+";
	char *popname;
	char *popip;

	popname = strtok(popserver, delims);
	popip = strtok(NULL, delims);

	if(user[0] == 0)
		http_fatal("error occur");

	if(popname == 0)
		http_fatal("error occur");

	if(popip == 0)
		http_fatal("error occur");

	char email[60];
	snprintf(email, 60, "%s@%s", user, popname);  // ע�ⲻҪ��emailŪ�����
	str_to_lowercase(email);

	printf("<center><table><td><td><pre>\n");
/*
	count=read_active(userid, &act_data);
	if (count<1) {
		http_fatal("���֣����޴��ˣ�������û�id��?");
	}
	if (strcmp(act_data->email, email)) {
		http_fatal("���û������ǲ����������������֤��ѽ>__<");
	}
*/
	int result;
	if (!strcasecmp(popname, "idp.xjtu6.edu.cn") && !strcasecmp(popip, IP_POP[3])) {
		if (!strcmp(fromhost, "202.117.1.190") || !strcmp(fromhost, "2001:250:1001:2::ca75:1be"))
			result=1;
		else {
			http_fatal("�ǿ��ŵ���֤��!");
			result=0;
		}
	}
	else {
		int pop_n;
		result = 0;
		for(pop_n=1;pop_n<=2; ++pop_n) {
			if(strcasecmp(popname, MAIL_DOMAINS[pop_n])==0 && strcasecmp(popip, IP_POP[pop_n])==0) {
				result = 1;
				break;
			}
		}

		if(result!=1)
			http_fatal("error occur");

		result = test_mail_valid(user, pass, popip);
	}

	switch (result)
	{
	case -1:
	case 0:
		http_fatal("�ʼ�������������ʧ�ܣ��޷���ѯ�����Ƿ��������������?");
		break;

	case 1:
		str_to_lowercase(email);
		struct associated_userid *au = get_associated_userid(email);
		if (au == NULL) {
			http_fatal("��ѯʧ��");
		}

		printf("<br>�����乲������%ld���û�.<br>\n", au->count);
		//�г�ͬ��¼�µ�����id
		for (i=0; i < au->count; ++i) {
			printf("%s %s<br>\n", au->id_array[i], style_to_str(au->status_array[i]));
		}
		free_associated_userid(au);
		break;
	}

	printf("<center><form><input type=button onclick='window.close()' value=�رձ�����></form></center>\n");

	return 0;
}

