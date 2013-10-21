#ifndef __APP_H
#define __APP_H
#include "ythtbbs.h"

/** bmy app �ṹ��
 * ��ź�app�йص�ԭʼ����
 */
struct bmyapp_s {
    char appkey[APPKEYLENGTH];				//!< app key����������ÿһ�� app��app key һ�������޷������������Ҫ��֤Ψһ��
    char secretkey[MAXSECRETKEYLENGTH];     //!< ˽Կ������urlǩ����֤
    char userid[IDLEN + 4];                 //!< app ��ʹ���� \@warning һ����˵bmy������ʹ�� IDLEN+2������ʹ�� IDLEN+4��ȷ������
    char appname[APPNAMELENGTH];            //!< app ������
    time_t createtime;                      //!< Ӧ�õĴ���ʱ��
    unsigned int status;                    //!< app ��״̬
};

/** appid hash table
 * λ�ڹ����ڴ��У����ڿ����������� appid
 */
struct appidhashitem {
	int num;
	char appkey[APPKEYLENGTH];
	char appname[APPNAMELENGTH];
};

char *generate_api_key();
char *generate_api_secret_key();

int create_new_app(char *appname, char *userid, char *);
struct bmyapp_s *get_app(char *appname);
#endif
