/* notification.c */
#ifndef __NOTIFICATION_H
#define __NOTIFICATION_H

enum {
	NOTIFY_TYPE_POST = '0'
};

struct NotifyItem {
	char from_userid[16];
	char time_str[32];		 // e.g. Thu Sep 12 18:28:57 2013
	char *link_gbk;
	struct NotifyItem * next;
};

typedef struct NotifyItem* NotifyItemList;

/** ����һ����������
 *
 * @param to_userid ֪ͨ�ʹ��id
 * @param from_userid ֪ͨ���Ե�id
 * @param board �������ڵİ���
 * @param article_id ������ʱ���
 * @param title_utf8 ���ӵı��� @warning utf8���룬���� libxml2 �����л����
 * @return ��ӳɹ�����0
 * @see is_post_in_notification(char * userid, char * board, int article_id)
 * @see del_post_notification(char * userid, char * board, int article_id)
 */
int add_post_notification(char * to_userid,
						  char * from_userid,
						  char * board,
						  int article_id,
						  char * title_utf8);

/** ��֪ͨ�������ڴ���
 *
 * @param userid �û� id
 * @return struct NotifyItem ����
 */
NotifyItemList parse_notification(char *userid);

/** �����û���֪ͨ����
 *
 * @param userid �û� id
 * @return ֪ͨ��������ֵ >= 0
 */
int count_notification_num(char *userid);

/** ����ĳƪ�����Ƿ�����Ϣ�б���
 * �÷�����ȷ���Ƿ���Ҫ��
 * @param userid �û� id
 * @param board ��������
 * @param article_id ����id
 * @return �����Ӵ�����֪ͨ�ļ��У��򷵻�1�����򷵻�0
 * @see del_post_notification(char * userid, char * board, int article_id)
 */
int is_post_in_notification(char * userid, char * board, int article_id);

/** ɾ��һ����������
 *
 * @param userid
 * @param board
 * @param article_id
 * @return <ul><li>0: ɾ���ɹ�</li><li>-1: ���Ӳ��������ļ���</li><li>-2: ɾ��ʧ��</li></ul>
 * @see is_post_in_notification(char * userid, char * board, int article_id)
 */
int del_post_notification(char * userid, char * board, int article_id);

#endif
