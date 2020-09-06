#ifndef BMY_MYSQL_WRAPPER
#define BMY_MYSQL_WRAPPER
#include <mysql/mysql.h>

enum MYSQL_STATUS {
	MYSQL_CANNOT_CONNECT_TO_MYSQL        = -1,
	MYSQL_CANNOT_INIT_STMT               = -2,
	MYSQL_CANNOT_BIND_PARAMS             = -3,
	MYSQL_CANNOT_BIND_RESULT_COLS        = -4,
	MYSQL_CANNOT_EXEC_STMT               = -5,
	MYSQL_CANNOT_STORE_RESULT            = -6,
	MYSQL_OK                             = 0
};

typedef void (*BMY_MYSQL_WRAPPER_CALLBACK)(MYSQL_STMT *stmt, MYSQL_BIND *result_cols, void *result_set);

/**
 * ʹ�� mysql prepared statement ���� sql ��ѯ�ĺ�����װ
 *
 * ִ�в�ѯ��������ȫ�������ڿͻ��ˣ������Ҫ�����ѯ�Ĺ�ģ�������ڲ������˽������ӡ���ѯ�������ͷ���Դ���������ʹ���ɻص��������������ػص�������ִ�н����
 * @param sqlbuf ��ѯ�� sql ��䣨�� prepared statement ��ʽд�룩
 * @param params �󶨵Ĳ��������������� NULL
 * @param result_cols �󶨵ķ���ֵ������������ NULL���ᱻ���� callback
 * @param result_set ���ڴ�ŷ��ؽ����ָ�룬�ᱻ���� callback
 * @param callback �����ѯ����Ļص�����
 * @return ���� mysql ����״̬
 */
int execute_prep_stmt(const char* sqlbuf, MYSQL_BIND *params, MYSQL_BIND *result_cols, void *result_set, BMY_MYSQL_WRAPPER_CALLBACK callback);

#endif
