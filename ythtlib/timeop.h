/* timeop.h */
#ifndef __TIMEOP_H
#define __TIMEOP_H
char *Ctime(time_t);

/** �Ƚϵ�ǰʱ���Ŀ��ʱ��Ĳ��죬�������ʵ����ַ�����
 * ���� 5����ǰ��10���Ӻ�
 * @param compared_time ��Ҫ�Աȵ�ʱ�䡣
 * @return ��������������
 * @warning �÷��������̰߳�ȫ�ģ����һ�ǿ��ת��Ϊ int ���͡�
 */
char *Difftime(time_t compared_time);

/**
 * @brief ���ĳһ�� 0:00:00 ��ʱ���(UTC+8 ʱ��)
 * @param tm
 * @return ʱ���
 */
time_t get_time_of_the_biginning_of_the_day(struct tm *tm);
#endif
