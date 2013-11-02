/* user.c */
#ifndef __USER_H
#define __USER_H
/** Structure used to hold information in PASSFILE
 *
 */
struct userec {
	char userid[IDLEN + 2];
	time_t firstlogin;
	char lasthost[16];
	unsigned int numlogins;
	unsigned int numposts;
	char flags[2];
	char passwd[PASSLEN];   //!<���ܺ������
	char username[NAMELEN];
	unsigned short numdays;	//!<������¼������
	char unuse[30];
	time_t dietime;
	time_t lastlogout;
	char ip[16];
	char realmail[STRLEN - 16];
	unsigned userlevel;
	time_t lastlogin;
	time_t stay;
	char realname[NAMELEN];
	char address[STRLEN];
	char email[STRLEN];
	int signature;
	unsigned int userdefine;
	time_t notedate_nouse;
	int noteline_nouse;
	int notemode_nouse;
};

struct override {
	char id[13];
	char exp[40];
};

char mytoupper(unsigned char ch);
char *sethomepath(char *buf, const char *userid);
char *sethomefile(char *buf, const char *userid, const char *filename);
char *setmailfile(char *buf, const char *userid, const char *filename);
int saveuservalue(char *userid, char *key, char *value);
int readuservalue(char *userid, char *key, char *value, int size);
char *cexp(int);
char *cperf(int);
int countexp(struct userec *);
int countperf(struct userec *);
int countlife(struct userec *);
int userlock(char *userid, int locktype);
int userunlock(char *userid, int fd);
int checkbansite(const char *addr);

/** ����û�Ȩ��
 * �ķ����� nju09 ��ֲ���μ� int user_perm(struct userec *x, int level)��
 * @param x
 * @param level
 * @return
 */
int check_user_perm(struct userec *x, int level);

extern struct user_info;
extern struct boardmem;
/**
 * @brief ����û����Ķ�Ȩ��
 * �÷����� nju09 ��ֲ�����ڼ���û���ȡ�����Ȩ�ޡ�
 * @param user
 * @param board ��������
 * @return
 * @see int has_read_perm(struct userec *user, char *board)
 * @see int has_read_perm_x(struct userec *user, struct boardmem *x)
 */
int check_user_read_perm(struct user_info *user, char *board);

/**
 * @brief ����û����Ķ�Ȩ��
 * �÷����� nju09 ��ֲ�����ڼ���û���ȡ�����Ȩ�ޡ�
 * @param user
 * @param board boardmem ָ��
 * @return
 * @see int has_read_perm(struct userec *user, char *board)
 * @see int has_read_perm_x(struct userec *user, struct boardmem *x)
 */
int check_user_read_perm_x(struct user_info *user, struct boardmem *board);
int userbansite(const char *userid, const char *fromhost);
void logattempt(char *user,char *from,char *zone,time_t time);
int inoverride(char *who, char *owner, char *file);
#endif
