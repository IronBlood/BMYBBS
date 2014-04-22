/* misc.c */
#ifndef __MISC_H
#define __MISC_H
extern int pu;
void getrandomint(unsigned int *s);
void getrandomstr(unsigned char *s);
/**
 * getrandomstr �����ı���
 * @param s �ַ���
 * @param len ����
 */
void getrandomstr_r(unsigned char *s, size_t len);
struct mymsgbuf {
	long int mtype;
	char mtext[1];
};
void newtrace(char *s);
int init_newtracelogmsq();
int u2g(char *inbuf,int inlen,char *outbuf,int outlen);
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen);
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
int is_utf_special_byte(unsigned char c);
int is_utf(char * inbuf, size_t inlen);

/**
 * @brief ���εķ���
 * ���� nju09
 * @param salt
 */
void getsalt(char salt[3]);

int badstr(unsigned char *s);

/**
 * @brief ��ȡ������4�Ž�վͼƬ
 * �÷������ѵ�ǰ��վͼƬ����Ϣ����� pics_list �С�ÿ����Ϣʹ�õ�����ǷֺŽ�ͼƬ��
 * ���Ӹ�����������Ϣʹ��������ǷֺŸ�����
 * @warning �ú��������� 2011.09.05 �� IronBlood ��д�� nju09/bbsindex.c �С�Ϊ��
 * api ���ã��������� libythtlib/misc.h �У�������˺���ԭ�͡�
 * @param pics_list �ַ�������������ҪԤ�ȷ���
 * @param len pics_list �ĳ���
 */
void get_no_more_than_four_login_pics(char *pics_list, size_t len);
#endif
