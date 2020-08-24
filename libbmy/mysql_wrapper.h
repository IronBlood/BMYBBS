#ifndef BMY_MYSQL_WRAPPER
#define BMY_MYSQL_WRAPPER
#include <mysql/mysql.h>

typedef void *(*BMY_MYSQL_WRAPPER_CALLBACK)(MYSQL_STMT *stmt, MYSQL_BIND *result_cols);

/**
 * ʹ�� mysql prepared statement ���� sql ��ѯ�ĺ�����װ
 *
 * ִ�в�ѯ��������ȫ�������ڿͻ��ˣ������Ҫ�����ѯ�Ĺ�ģ�������ڲ������˽������ӡ���ѯ�������ͷ���Դ���������ʹ���ɻص��������������ػص�������ִ�н����
 * @param sqlbuf ��ѯ�� sql ��䣨�� prepared statement ��ʽд�룩
 * @param params �󶨵Ĳ��������������� NULL
 * @param result_cols �󶨵ķ���ֵ������������ NULL
 * @param callback �����ѯ����Ļص�����
 * @return ���� callback �ķ���ֵ
 */
void *execute_prep_stmt(const char* sqlbuf, MYSQL_BIND *params, MYSQL_BIND *result_cols, BMY_MYSQL_WRAPPER_CALLBACK callback);

#endif
