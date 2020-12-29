#include "bbslib.h"

extern bool nju09_mybrd_has_read_perm(const char *userid, const char *boardname);

int
bbsbrddel_main()
{
	FILE *fp;
	char file[200], board[200];
	int i;
	html_header(1);
	changemode(ZAP);
	ytht_strsncpy(board, getparm("B"), 32);
	if (!board[0])
		ytht_strsncpy(board, getparm("board"), 32);
	if (!loginok)
		http_fatal("��ʱ��δ��¼��������login");
	readmybrd(currentuser.userid);
	if (g_GoodBrd.num == 0)
		http_fatal("��û��Ԥ���κ�������");
	if (!ythtbbs_mybrd_exists(&g_GoodBrd, board))
		http_fatal("�㲢û��Ԥ�����������");
	ythtbbs_mybrd_remove(&g_GoodBrd, board);
	ythtbbs_mybrd_save(currentuser.userid, &g_GoodBrd, nju09_mybrd_has_read_perm);
	printf("<script>top.f2.location.reload();</script>\n");
	printf("�Ƴ�Ԥ���������ɹ�<br><a href='javascript:history.go(-1)'>���ٷ���</a>");
	http_quit();
	return 0;
}
