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
    unsigned int permission;				//!< api Ȩ��
    struct bmyapp_s *next;					//!< ������ struct bmyapp_s * ������ֵ���ļ��в��洢���ݡ�
};

enum bmyapp_s_status {
	BMYAPP_STATUS_INITIAL	= 1,			//!< վ����ʼ������
	BMYAPP_STATUS_CREATED	= 2,			//!< ������ԱӦ�ô������
	BMYAPP_STATUS_PUBLISHED	= 3,			//!< վ�����ͨ��������
	BMYAPP_STATUS_FORBIDDEN	= 4,			//!< Ӧ�ñ����ã�Ӧ��ӵ���߿��Խ���Ӧ�ã�����ֻ��վ�����Իָ��� PUBLISHED ״̬
	BMYAPP_STATUS_DELETED	= 5,			//!< ɾ��״̬�����ǲ�ɾ��Ӧ����Ϣ��ȷ�� APPKEY ���ظ����κ��˲��ܴ���ɾ��״̬�ָ���
};

/** appid hash table
 * λ�ڹ����ڴ��У����ڿ����������� appid
 */
struct appidhashitem {
	int num;
	char appkey[APPKEYLENGTH];
	char appname[APPNAMELENGTH];
};

/**
 * @brief ���� appkey
 * @return
 */
char *generate_app_key(struct bmyapp_s *app);

/**
 * @brief ���� app ��˽Կ
 * @return
 */
char *generate_app_secret_key(struct bmyapp_s *app);

/**
 * @brief ����Ӧ��
 * @param appname Ӧ������
 * @param applyuserid Ӧ�������˵� id
 * @param appoveuserid Ӧ����׼�˵� id
 * @param permission Ӧ��Ȩ��
 * @return �ɹ����� 0�����򷵻ظ���
 */
int create_new_app(char *appname, char *applyuserid, char *appoveuserid, unsigned int permission);

enum bmyapp_query_mode {
	BMYAPP_QUERYMODE_APPKEY,
	BMYAPP_QUERYMODE_APPNAME,
	BMYAPP_QUERYMODE_USERID
};

/**
 * @brief ����Ӧ��
 * @param querystring ���ҵ��ַ���
 * @param querymode ���ҵ�ģʽ
 * @return
 */
struct bmyapp_s *get_app(char *querystring, int querymode);
#endif
