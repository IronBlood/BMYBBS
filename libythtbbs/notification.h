/* notification.c */
#ifndef __NOTIFICATION_H
#define __NOTIFICATION_H

enum {
	NOTIFY_TYPE_POST = '0'
};

/** ����һ����������
 *
 * @param to_userid ֪ͨ�ʹ��id
 * @param from_userid ֪ͨ���Ե�id
 * @param board �������ڵİ���
 * @param article_id ������ʱ���
 * @param title_utf8 ���ӵı��� @warning utf8���룬���� libxml2 �����л����
 * @return ��ӳɹ�����0
 */
int add_post_notification(char * to_userid,
						  char * from_userid,
						  char * board,
						  int article_id,
						  char * title_utf8);

#endif
