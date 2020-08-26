#ifndef _LIBIDEN__H
#define _LIBIDEN__H

#include <mysql/mysql.h>

//�����������ݿ��һЩ����
#define SQLDB "bbsreg"
#define SQLUSER "root"
#define SQLPASSWD "xj,bbs,1"
#define USERREG_TABLE "userreglog"
// #define SCHOOLDATA_TABLE "schooldata"

//һЩ����ֵ
#define TOO_MUCH_RECORDS 3
#define WRITE_SUCCESS    2
#define UPDATE_SUCCESS   1
#define WRITE_FAIL       0
#define FILE_NOT_FOUND  -1

//һ����¼�������id
#define MAX_USER_PER_RECORD 4

//�����������
#define DOMAIN_COUNT 3
extern const char *MAIL_DOMAINS[];
extern const char *IP_POP[];

//�󶨵ķ�ʽ
#define DIED_ACIVE -1 /* ����*/
#define NO_ACTIVE      0  /*δ��*/
#define MAIL_ACTIVE    1  /*������֤*/
#define PHONE_ACTIVE   2  /*�ֻ���֤*/
#define IDCARD_ACTIVE  3  /*�ֹ��ϴ����֤*/
#define FORCE_ACTIVE   4  /*վ��ǿ�Ƽ���*/

//��֤��ĳ���
#define CODELEN 8
#define VALUELEN 80

struct active_data{
	char userid[IDLEN+2];
	char name[STRLEN];
	char dept[STRLEN];
	char ip[20];
	char regtime[32];
	char uptime[32];
	char operator[IDLEN+2];
	char email[VALUELEN];
	char phone[VALUELEN];
	char idnum[VALUELEN];
	char stdnum[VALUELEN];
	int status;
};

struct associated_userid {
	size_t count;
	char **id_array;
	int  *status_array;
};

char* str_to_uppercase(char *str);
char* str_to_lowercase(char *str);
const char* style_to_str(int style);
//int send_active_mail(char* mbox, char* code,char* userid, session_t* session);

int query_record_num(char* value, int style);
int write_active(struct active_data* act_data);

int read_active(char* userid, struct active_data* act_data);
int get_active_value(char* value, struct active_data* act_data);

MYSQL * my_connect_mysql(MYSQL *s);

/**
 * ���������ַ��ѯ������id�б�ʹ�ý����ǵõ��� free_associated_userid �ͷ��ڴ档���������� get_associated_userid_by_style �ķ�װ��
 * @param email �ʼ���ַ
 * @return struct associated_userid ָ��
 */
struct associated_userid *get_associated_userid(const char *email);

/**
 * ������֤��ʽ������id�б�ʹ�ý����ǵõ��� free_associated_userid �ͷ��ڴ档
 * @param style ��֤��ʽ��MAIL_ACTIVE | PHONE_ACTIVE | IDCARD_ACTIVE | FORCE_ACTIVE
 * @param value ����ֵ
 * @return struct associated_userid ָ��
 */
struct associated_userid *get_associated_userid_by_style(int style, const char *value);

/**
 * �ͷ��� get_associated_userid �������ڴ�ռ䡣
 * @param au
 */
void free_associated_userid(struct associated_userid* au);
#endif

