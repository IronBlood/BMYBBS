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
/**
 * ���� BMY_ID�����䷢����֤�룬����ֵ status json
 * ���������
 *   - userid �ַ�����BMY_ID
 *   - user �ַ�����������
 *   - domain �ַ���������
 * RETURN:
 *   - -1: ��������ȷ
 *   - -2: ���޴���
 *   - -3: ���䲻ƥ��
 *   - -4: ������֤�����
 *   - 0: �ɹ�����
 */
void api_bbsresetpass(void) {
	char *userid = getparm("userid");
	char *user   = getparm("user");
	char *domain = getparm("domain");
	char email[80];
	int rc = 0;
	int domain_idx;
	struct active_data act_data;

	if (userid == NULL || strlen(userid) < 2 || strlen(userid) > IDLEN
		|| user == NULL || strlen(user) == 0 || strlen(user) > 20
		|| domain == NULL || strlen(domain) != 1) {
		rc = -1;
		goto OUTPUT;
	}

	domain_idx = domain[0] - '0';
	if (domain_idx < 0 || domain_idx > 3) domain_idx = 0;
	snprintf(email, 80, "%s@%s", user, MAIL_DOMAINS[domain_idx]);
	str_to_lowercase(email);

	rc = read_active(userid, &act_data);
	if (rc < 1) {
		rc = -2;
		goto OUTPUT;
	}

	if (strcasecmp(act_data.email, email) != 0) {
		rc = -3;
		goto OUTPUT;
	}

	rc = send_resetpass_mail(act_data.userid, email);
	if (rc != 1) {
		rc = -4;
		goto OUTPUT;
	}

	rc = 0;
OUTPUT:
	json_header();
	printf("{ \"status\": %d }", rc);
}

/**
 * ��������
 * ���������
 *   - userid �ַ�����BMY_ID
 *   - pass1  �ַ���������
 *   - pass2  �ַ���������
 *   - code   �ַ�������֤��
 * RETURN
 *   - -1: ��������
 *   - -2: ���޴���
 *   - -3: ��֤�����
 *   - 0: ���óɹ�
 */
void api_do_bbsresetpass(void) {
	char *userid = getparm("userid");
	char *pass1  = getparm("pass1");
	char *pass2  = getparm("pass2");
	char *code   = getparm("code");

	struct userec *x;
	char salt[3];
	int rc;

	if (userid == NULL || strlen(userid) < 2 || strlen(userid) > IDLEN
		|| pass1 == NULL || pass2 == NULL || strlen(pass1) < 1 || strlen(pass1) > 13 || strcmp(pass1, pass2) != 0
		|| code == NULL || strlen(code) != 5) {
		rc = -1;
		goto OUTPUT;
	}

	x = getuser(userid);
	if (x == NULL) {
		rc = -2;
		goto OUTPUT;
	}

	rc = verify_captcha_for_user(x->userid, code, CAPTCHA_FILE_RESET);
	if (rc != CAPTCHA_OK) {
		rc = -3;
		goto OUTPUT;
	}

	getsalt(salt);
	strcpy(x->passwd, crypt1(pass2, salt));
	save_user_data(x);
	rc = 0;
OUTPUT:
	json_header();
	printf("{ \"status\": %d }", rc);
}

int
bbsresetpass_main()
{
	char *type = getparm("type");

	if (strcmp(type, "1") == 0) {
		api_bbsresetpass();
		return 0;
	} else if (strcmp(type, "2") == 0) {
		api_do_bbsresetpass();
		return 0;
	}

	http_fatal("������ˣ�");
	return 0;
}

