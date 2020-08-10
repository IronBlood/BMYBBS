/* board.c */
#ifndef __BOARD_H
#define __BOARD_H

#define BMNUM 16
struct boardheader {		/* This structure is used to hold data in */
	char filename[24];	/* the BOARDS files */
	char title[24];
	int clubnum;
	unsigned level;
	char flag;
	char secnumber1;	//������Ǵ˰������ڵ����ţ����磺'2','G'�ȵ�
	char secnumber2;
	char type[5];
	char bm[BMNUM][IDLEN + 1];
	int hiretime[BMNUM];
	int board_ctime;
	int board_mtime;
	char sec1[4];
	char sec2[4];
	char keyword[64];
	char unused[96];
};

struct boardmem {		/* used for caching files and boards */
	struct boardheader header;
	int lastpost;
	int total;
	short inboard;
	short bmonline;
	short bmcloak;
	int stocknum;
	int score;
	int unused[10];
};

struct boardmanager {		/* record in user directionary */
	char board[24];
	char bmpos;
	char unused;
	short bid;
};

char *bm2str(char *buf, struct boardheader *bh);
char *sbm2str(char *buf, struct boardheader *bh);

/**
 * ���ݰ������ƻ�ȡ boardmem ����
 * �� nju09/BBSLIB.c ���ƶ�������Ӧ���� libythtbbs ���һ���֡��÷������� shm_bcache
 * �еݹ�Ĳ��ұȶ� board_name������ͬ�򷵻� boardmem ��ַ��
 * @warning �÷����в������û�Ȩ�޵�У�顣���ý�������Ҫ�ͷ� boardmem ��ַ��
 * @param board_name �����Ӣ������
 * @see struct boardmem * getbcache(char *board)
 * @see struct boardmem * getboard(char *board)
 */
struct boardmem *getboardbyname(const char *board_name);  // �� nju09 ��ֲ��by IronBlood 20130805

/**
 * @brief ���ݰ������Ƽ���Ƿ��Ǵ����� etc/junkboards �ļ���
 * @warning �÷�����ֲ�� nju09���� term �����в�ͬ����Ҫ��һ����顣
 * @param board_name ��������
 * @return
 */
int board_is_junkboard(char *board_name);
#endif
