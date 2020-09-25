#include "bbslib.h"
#include "ythtbbs/identify.h"

static void check_captcha_form(void);
static void resent_active_mail(void);

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

#ifdef POP_CHECK
void register_success(int usernum, char *userid, char *realname, char *dept,
char *addr, char *phone, char *assoc, char *email);
#endif

int
bbsform_main()
{
	int type;

	type = atoi(getparm("type"));

	if (!loginok || isguest) {
		http_fatal("����δ��¼, �����µ�¼��");
	}

	check_if_ok();

	if (type == 1) {
		// update register info
		check_submit_form();
		http_quit();
	} else if (type == 2) {
		// check captcha
		check_captcha_form();
		http_quit();
	} else if (type == 3) {
		// resent captcha
		resent_active_mail();
		http_quit();
	}


	html_header(1);
	changemode(NEW);
	printf("%s -- ��дע�ᵥ<hr>\n", BBSNAME);
	printf("<link href='/bbsform.css' rel='stylesheet'>");
	printf("����, %s, ע�ᵥͨ���󼴿ɻ��ע���û���Ȩ��, ������������������д<br><hr>\n", currentuser.userid);
	printf("<div id='form-reg' class='hidden'>");
	printf("<form method=post action=bbsform?type=1>\n");
	printf("��ʵ����: <input name=realname type=text maxlength=8 size=8 value='%s'><br>\n", nohtml(currentuser.realname));
	printf("��ס��ַ: <input name=address type=text maxlength=32 size=32 value='%s'><br>\n", nohtml(currentuser.address));
	printf("����绰(��ѡ): <input name=phone type=text maxlength=32 size=32><br>\n");
	printf("ѧУϵ��/��˾��λ: <input name=dept maxlength=60 size=40><br>\n");
	printf("У�ѻ�/��ҵѧУ(��ѡ): <input name=assoc maxlength=60 size=42><br><hr><br>\n");

#ifdef POP_CHECK
	printf("������ϢҪ��Ϊ�ʼ������������֤֮�ã�������д<br><hr>\n");
	printf("ÿ��������������֤ %d ��bbs�ʺ� <br><hr>\n", MAX_USER_PER_RECORD);
	printf("<tr><td align=right>*�������ε��ʼ��������б�:<td align=left><SELECT NAME=popserver>\n");
	int n = 1;
	while(n <= DOMAIN_COUNT)
	{
		if (n == 1)
			printf("<OPTION VALUE=%d SELECTED>", n);
		else
			printf("<OPTION VALUE=%d>", n);

		printf("%s", MAIL_DOMAINS[n]);
		n++;
	}
	printf("</select><br>\n");
	printf("<tr><td align=right>*�����������û���:<td align=left><input name=user size=20 maxlength=20 placeholder='ֻ��Ҫ����@֮ǰ�Ĳ���'><br>\n");

#endif

	printf("</form>");
	printf("<button id='btn-reg-submit'>ע��</button> <button id='btn-reg-reset'>ȡ��</button> <button id='btn-reg-captcha'>������֤��</button>");
	printf("</div>"); // div#form-reg

	printf("<div id='form-cap' class='hidden'>");
	printf("�������������ǣ� <span id='cap-email'>%s</span>����֤���ѷ��͵��������䡣<br>", nohtml(currentuser.email));
	printf("��֤��<input name='captcha' maxlength=5><br>");
	printf("<button id='btn-cap-submit'>�ύ</button> <button id='btn-cap-resent'>���·���</button> <button id='btn-cap-update'>��������</button>");
	printf("</div>"); //div#form-cap

	printf("<div id='myModal' class='modal'>");
	printf("<div class='modal-content'>");
	printf("<span class='close'>&times;</span>");
	printf("<p></p>");
	printf("</div></div>");
	printf("<script>var cap_status=%d;</script>", check_captcha_status(currentuser.userid, CAPTCHA_FILE_REGISTER)); // CAPTCHA_OK == 0
	printf("<script src='/bbsform.js'></script>");
	http_quit();
	return 0;
}

void
check_if_ok()
{
	if (user_perm(&currentuser, PERM_LOGINOK))
		http_fatal("���Ѿ�ͨ����վ�������֤, �����ٴ���дע�ᵥ.");

	if (has_fill_form())
		http_fatal("Ŀǰվ����δ��������ע�ᵥ�������ĵȴ�.");
}

void
check_submit_form()
{

	FILE *fp;
	char dept[80], phone[80], assoc[80];
	struct active_data act_data;
	int count, mail_diff;
	int isprivilege = 0;
	char path[128];

#ifdef POP_CHECK
	char user[USER_LEN + 1];
	char pass[PASS_LEN + 1];
	char popserver[4];
	int popserver_index;
	ytht_strsncpy(popserver, getparm("popserver"), 4);
	ytht_strsncpy(user, getparm("user"), USER_LEN);
	ytht_strsncpy(pass, getparm("pass"), PASS_LEN);

	if (strlen(user) == 0)
		http_fatal("�����û���û��");

	const char *popname;
	popserver_index = atoi(popserver);
	if(popserver_index < 1 || popserver_index > 3) popserver_index = 1;
	popname = MAIL_DOMAINS[popserver_index];

	char email[60];
	sprintf(email, "%s@%s", user, popname);  // ע�ⲻҪ��emailŪ�����
	str_to_lowercase(email);
	count = read_active(currentuser.userid, &act_data);
	if (count == 0) {
		mail_diff = 1;
	} else {
		if (strcasecmp(act_data.email, email) != 0) {
			mail_diff = 1;
		}
	}
	ytht_strsncpy(currentuser.email, email, 60);

	if (check_mail_to_address(email) == MAIL_SENDER_WRONG_EMAIL)
		http_fatal("�������������Ϸ�������ϵվ������ https://github.com/bmybbs/bmybbs/issues/ �������⡣");
#endif

	ytht_strsncpy(currentuser.realname, getparm("realname"), 20);
	ytht_strsncpy(dept, getparm("dept"), 60);
	ytht_strsncpy(currentuser.address, getparm("address"), 60);
	ytht_strsncpy(phone, getparm("phone"), 60);
	ytht_strsncpy(assoc, getparm("assoc"), 60);
	memset(&act_data, 0, sizeof(act_data));
	snprintf(act_data.name, NAMELEN, "%s", currentuser.realname);
	act_data.name[NAMELEN-1] = '\0';
	strcpy(act_data.userid, currentuser.userid);
	snprintf(act_data.dept, STRLEN, "%s", dept);
	act_data.dept[STRLEN-1] = '\0';
	snprintf(act_data.phone, VALUELEN, "%s", phone);
	act_data.phone[VALUELEN-1] = '\0';
	snprintf(act_data.email, VALUELEN, "%s", email);
	act_data.email[VALUELEN-1] = '\0';
	strcpy(act_data.ip, currentuser.lasthost);
	strcpy(act_data.operator, currentuser.userid);
	act_data.status=0;
	write_active(&act_data);

#ifndef POP_CHECK
	fp = fopen("new_register", "a");
	if (fp == 0)
		http_fatal("ע���ļ�������֪ͨSYSOP");
	fprintf(fp, "usernum: %d, %s\n", getusernum(currentuser.userid) + 1,
		Ctime(now_t));
	fprintf(fp, "userid: %s\n", currentuser.userid);
	fprintf(fp, "realname: %s\n", currentuser.realname);
	fprintf(fp, "dept: %s\n", dept);
	fprintf(fp, "addr: %s\n", currentuser.address);
	fprintf(fp, "phone: %s\n", phone);
	fprintf(fp, "assoc: %s\n", assoc);
	fprintf(fp, "----\n");
	fclose(fp);
	printf("����ע�ᵥ�ѳɹ��ύ. վ�����������������, ��������������.<br>" "<a href=bbsboa>���" MY_BBS_NAME "</a>");
#else
	int result;

	snprintf(path, 127, MY_BBS_HOME "/etc/pop_register/%s", popname);
	if (seek_in_file(path, user)) {
		isprivilege = 1;
	}

	if (strcasecmp(user, "test") == 0) {
		result = -2;
	} else if (isprivilege == 0 && query_record_num(email, MAIL_ACTIVE) >= MAX_USER_PER_RECORD) {
		result = -3;
	} else {
		if (mail_diff) {
			unlink_captcha(currentuser.userid, CAPTCHA_FILE_REGISTER);
		}
		result = send_active_mail(currentuser.userid, email);
	}

	html_header(1);

	switch (result)
	{
	default:
	case -2:
	case -1:
	case 0:
		printf("�ʼ�������������ʧ�ܣ�����ֻ��ʹ�ñ�bbs����������ܣ�ʮ�ֱ�Ǹ��<br>");
		break;

	case -3:
		printf("���������Ѿ���֤�� %d ��id���޷���������֤��!\n", MAX_USER_PER_RECORD);
		break;
	case 1:
		printf("��֤��Ϣ�ѷ������������� %s ����ʱ����ա�<br>"
			"���¼ϵͳ��������������дע�ᵥ��������������֤��������Ϊ��վ��ʽ�û���", email);
		break;
	}

#endif

}

static void
check_captcha_form(void)
{
	char code[6];
	char tmp_email[STRLEN+1], *domain, path[128];
	int rc, isprivilege;
	struct active_data act_data;

	memset(&act_data, 0, sizeof(struct active_data));
	snprintf(code, 6, "%s", getparm("code"));
	rc = verify_captcha_for_user(currentuser.userid, code, CAPTCHA_FILE_REGISTER);
	if (rc == CAPTCHA_OK) {
		read_active(currentuser.userid, &act_data);

		snprintf(tmp_email, STRLEN, "%s", act_data.email);
		domain = strchr(tmp_email, '@');
		if (domain != NULL) {
			*domain = '\0';
			domain++;
			snprintf(path, 127, MY_BBS_HOME "/etc/pop_register/%s", domain);

			if (seek_in_file(path, tmp_email)) {
				isprivilege = 1;
			}
		}
		if ((isprivilege == 1) || (query_record_num(act_data.email, MAIL_ACTIVE) < MAX_USER_PER_RECORD)) {
			act_data.status = 1;
			rc = write_active(&act_data);
			if (rc == WRITE_SUCCESS || rc == UPDATE_SUCCESS) {
				register_success(getusernum(currentuser.userid) + 1, currentuser.userid, currentuser.realname,
						act_data.dept, currentuser.address, act_data.phone, "", act_data.email);
				rc = 0;
			} else {
				// WRITE_FAIL == 0
				rc = -1;
			}
		} else {
			rc = -1;
		}
	}

	json_header();
	printf("{ \"status\": %d }", rc);
}

static void
resent_active_mail(void)
{
	int rc;
	char c;

	if (strlen(currentuser.email) < 5) {
		rc = -2;
	} else {
		c = currentuser.email[4];
		currentuser.email[4] = '\0';
		rc = strcasecmp(currentuser.email, "test");
		currentuser.email[4] = c;
		if (rc == 0) {
			rc = -2;
		} else if (query_record_num(currentuser.email, MAIL_ACTIVE) >= MAX_USER_PER_RECORD) {
			rc = -3;
		} else {
			rc = send_active_mail(currentuser.userid, currentuser.email);
		}
	}

	json_header();
	printf("{ \"status\": %d }", rc);
}

