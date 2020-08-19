#ifndef BMYBBS_MAILSENDER_H
#define BMYBBS_MAILSENDER_H

/**
 * ״̬��
 */
enum mail_sender_code {
	MAIL_SENDER_SUCCESS = 0,
	/* enum_status_code */
	MAIL_SENDER_WRONG_EMAIL = 20,
	MAIL_SENDER_CONFIG_NOT_EXIST = 21,
	MAIL_SENDER_CONFIG_ERROR = 22,
	/* not a valid status code */
	MAIL_SENDER__LAST
};

/**
 * ���͵����ʼ�
 * @param mail_to         �ռ�������
 * @param mail_to_name    �ռ������֣�����ʹ�� bmy id��
 * @param mail_subject    �ʼ����⣨utf-8 ���룩
 * @param mail_body       �ʼ����ģ�utf-8 ���룩
 * @return ״̬�� @ref mail_sender_code
 */
enum mail_sender_code send_mail(const char *mail_to, const char *mail_to_name, const char *mail_subject, const char *mail_body);
#endif //BMYBBS_MAILSENDER_H
