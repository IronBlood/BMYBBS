#include "ythtbbs/commend.h"
#include "bbslib.h"
//#include "struct.h"
#define SHOWBOARDS 5    //add by lsssl@072706 you will see "SHOWBOARDS" boards every section at the firstlook of bmy;

#define AREA_DIR		"etc/Area_Dir"	// ÿ���������Ż����ļ��Ĵ��Ŀ¼
static int showboardlist(struct boardmem *(data[]), int total, char *secstr, const struct sectree *sec);
static int showsecpage(const struct sectree *sec, struct boardmem *(data[]), int total, char *secstr);
static int showdefaultsecpage(const struct sectree *sec, struct boardmem *(data[]), int total, char *secstr);
static int showsecnav(const struct sectree *sec);
static int showstarline(char *str);
static int showhotboard(const struct sectree *sec, char *s);
static int showsechead(const struct sectree *sec);
int board_read(char *board, int lastpost);
void printlastmark(char *board);
static int showsecmanager(const struct sectree *sec);

void show_area_top(char c);

void show_banner();
int show_commend();
int show_content();
void show_sec(const struct sectree *sec);
void show_sec_by_name(char secid);
void show_boards(const char* secstr);
void show_sec_boards(struct boardmem *(data[]), int total);
void show_top10();
void show_right_click_header(int i);
// int show_manager_team(); δʹ��

// bbsmybrd.c
int ismybrd(char *board);
// bbsshownav
int shownavpart(int mode, const char *secstr);

int
bbsboa_main()
{
	struct boardmem *(data[MAXBOARD]);
	int i, total = 0;
	char *secstr; //, session_name[STRLEN], pname[STRLEN], *p;
	const struct sectree *sec;
	//int show_Commend();
	int hasintro = 0, len;

	secstr = getparm("secstr");
	sec = getsectree(secstr);
	html_header(1);
	check_msg();
	//printf("<style type=text/css>A {color: #0000f0}</style>");
	changemode(SELECT);
	if (secstr[0] == '*') {
		readmybrd(currentuser.userid);
		ythtbbs_cache_Board_foreach_v(filter_board_v, FILTER_BOARD_check_mybrd, data, &total);
		printf("<body><center>\n");
		printf("<div class=rhead>%s --<span class=h11> Ԥ������������</span></div><hr>", BBSNAME);
		if (total)
			showboardlist(data, total, "*", sec);
		printf("<hr>");
		return 0;
	}
	if(!strcmp(secstr,"?")){
		printf("<script src=\"/inc/tog.js\"></script></head><body leftmargin=0 topmargin=0>\n");
		show_banner();
		show_content();
		goto out;
	}
	len = strlen(secstr);
	if (sec->introstr[0])
		hasintro = 1;
	ythtbbs_cache_Board_foreach_v(filter_board_v, FILTER_BOARD_with_secstr, data, &total, hasintro, secstr);
	printf("<body topmargin=0 leftMargin=1 MARGINWIDTH=1 MARGINHEIGHT=0>");
	showsecpage(sec, data, total, secstr);
out:
	printf("</body></html>");
	return 0;
}

static int showsecpage(const struct sectree *sec, struct boardmem *(data[]), int total, char *secstr) {
	FILE *fp;
	char buf[1024];
	sprintf(buf, "wwwtmp/secpage.sec%s", sec->basestr);
	fp = fopen(buf, "rt");
	if (!fp)
		return showdefaultsecpage(sec, data, total, secstr);
	while (fgets(buf, sizeof (buf), fp)) {
		if (buf[0] != '#') {
			fputs(buf, stdout);
			continue;
		}
		if (!strncmp(buf, "#showfile ", 10)) {
			char *ptr;
			ptr = strchr(buf, '\n');
			if (ptr)
				*ptr = 0;
			showfile(buf + 10);
		} else if (!strncmp(buf, "#showblist", 10)) {
			if (total)
				/* modified by freely@BMY@20060525 */
				showboardlist(data, total, secstr, sec);
				//showboardlist(data, total, secstr);
		} else if (!strncmp(buf, "#showsecnav", 11))
			showsecnav(sec);
		else if (!strncmp(buf, "#showstarline ", 14))
			showstarline(buf + 14);
		else if (!strncmp(buf, "#showhotboard", 13))
			showhotboard(sec, buf + 13);
		else if (!strncmp(buf, "#showsechead", 12))
			showsechead(sec);
		else if (!strncmp(buf, "#showsecmanager", 8))
			showsecmanager(sec);
	}
	fclose(fp);
	return 0;
}

//modify by mintbaggio 040522 for new www
static int showdefaultsecpage(const struct sectree *sec, struct boardmem *(data[]), int total, char *secstr) {
	printf("<tr><td height=30 colspan=2></td></tr>\n"
		"<tr><td height=70 colspan=2><table width=\"100%%\" border=0 cellpadding=0 cellspacing=0 bgcolor=#efefef>\n"
		"<tr><td width=40>&nbsp;</td>\n");
	printf("<td width=400 height=70><a href=\"boa?secstr=%s\" class=btnsubmittheme>%s </a> </td>", sec->basestr, nohtml(sec->title));
	printf("%s", "<td width=300><table width=164 border=0 cellpadding=0 cellspacing=0>\n"
	"<tr><td><input name=textfield type=text class=\"inputsearch\" size=20>\n"
	"</td>\n<td width=102><input name=Submit type=button class=sumbitgrey value=Search></td>\n"
	"<td width=102 height=20>&nbsp;</td></tr></table></td></tr>\n");
	showboardlist(data, total, secstr, sec);
	printf("</body></html>\n");
	return 0;
}

static int showsechead(const struct sectree *sec) {
	const struct sectree *sec1, *sec2;
	int i;
	sec2 = sec;
	while (sec2->parent && !sec2->introstr[0])
		sec2 = sec2->parent;
	printf("<table border=1 bgcolor=%s><tr>", currstyle->colortb1);
	if (sec == &sectree)
		printf("<td align=center>&nbsp;<b>%s</b>&nbsp;</td>", nohtml(sectree.title));
	else if (sec2 == &sectree)
		printf("<td align=center>&nbsp;<b><a href=boa?secstr=?>%s</a></b>&nbsp;</td>", nohtml(sectree.title));
	else
		printf("<td align=center>&nbsp;<a href=boa?secstr=?>%s</a>&nbsp;</td>", nohtml(sectree.title));
	for (i = 0; i < sectree.nsubsec; i++) {
		sec1 = sectree.subsec[i];
		if (!sec1->introstr[0])
			continue;
		if (sec1 == sec)
			printf("<td align=center>&nbsp;<b>%s</b>&nbsp;</td>", nohtml(sec1->title));
		else if (sec1 == sec2)
			printf("<td align=center>&nbsp;<b><a href=boa?secstr=%s>%s</a></b>&nbsp;</td>", sec1->basestr, nohtml(sec1->title));
		else
			printf("<td align=center>&nbsp;<a href=boa?secstr=%s>%s</a>&nbsp;</td>", sec1->basestr, nohtml(sec1->title));
	}
	printf("</tr></table>");
	return 0;
}

static int showstarline(char *str) {
	printf("<tr><td class=tb2_blk><font class=star>��</font>" "&nbsp;%s</td></tr>", str);
	return 0;
}

static int showsecnav(const struct sectree *sec) {
	char buf[256];
	printf("<table width=100%%>");
	sprintf(buf,
		"���վ��ʻ����Ƽ� &nbsp;(<a href=bbsshownav?secstr=%s class=blk>"
		"�鿴ȫ��</a>)", sec->basestr);
	showstarline(buf);
	printf("<tr><td>");
	shownavpart(0, sec->basestr);
	printf("</td></tr></table>");
	return 0;
}

// ��������Ͳ��� filter_board_v �ϲ��ˣ���Щ�����߼�
static int showhotboard_callback(struct boardmem *board, int curr_idx, va_list ap) {
	const char *basestr = va_arg(ap, const char *);
	struct boardmem **bmem = va_arg(ap, struct boardmem **);
	int *count = va_arg(ap, int *);
	int max = va_arg(ap, int);
	int len = strlen(basestr);
	int j;
	struct boardmem *x1;

	if (board->header.filename[0] <= 32 || board->header.filename[0] > 'z')
		return 0;

	if (hideboard_x(board))
		return 0;

	if (strncmp(basestr, board->header.sec1, len) && strncmp(basestr, board->header.sec2, len))
		return 0;

	for (j = 0; j < *count; j++) {
		if (board->score > bmem[j]->score)
			break;

		if (board->score == bmem[j]->score && board->inboard > bmem[j]->inboard)
			break;
	}

	for (; j < *count; j++) {
		x1 = bmem[j];
		bmem[j] = board;
		board = x1;
	}

	if (*count < max) {
		bmem[*count] = board;
		*count = *count + 1;
	}

	return 0;
}

static int showhotboard(const struct sectree *sec, char *s) {
	int count = 0, i, j, len, max;
	struct boardmem *bmem[MAXBOARD];
	max = atoi(s);
	if (max < 3 || max > 30)
		max = 10;
	len = strlen(sec->basestr);
	ythtbbs_cache_Board_foreach_v(showhotboard_callback, sec->basestr, bmem, &count, max);
	printf("<table width=588 border=1><tr><td bgcolor=%s width=55 align=center>�����������Ƽ�</td><td>", currstyle->colortb1);
	for (i = 0; i < count; i++) {
		if (i)
			printf("%s", " &nbsp;");
		printf("<a href=%s%s class=pur><u>%s</u></a>",
				showByDefMode(),
				bmem[i]->header.filename,
				void1(nohtml(bmem[i]->header.title)));
	}
	printf("</td></tr></table>");
	return 0;
}

int
showfile(char *fn)
{
	struct mmapfile mf = { .ptr = NULL };
	MMAP_TRY {
		if (mmapfile(fn, &mf) < 0) {
			MMAP_RETURN(-1);
		}
		fwrite(mf.ptr, 1, mf.size, stdout);
	}
	MMAP_CATCH {
	}
	MMAP_END mmapfile(NULL, &mf);
	return 0;
}

int
showsecintro(const struct sectree *sec)
{
	char filename[80];
	int i;
	if (!sec->introstr[0])
		return -1;
	sprintf(filename, "wwwtmp/lastmark.sec%s", sec->basestr);
	if (showfile(filename) < 0 && sec->nsubsec>0) {
		for (i = 0; i < sec->nsubsec; i++) {
			printf("<td class=tdborder>��</td>\n");
			printf("<td class=tduser><a href=bbsboa?secstr=%s>&nbsp;</a></td>", sec->subsec[i]->basestr);
			printf("<td class=tdborder>&nbsp;</td><td class=tdborder>[��������]</td>");
			printf("<td class=tdborder><a href=bbsboa?secstr=%s>%s</a></td>", sec->subsec[i]->basestr, sec->subsec[i]->title);
			printf("<td class=tdborder></td><td class=tdborder></td><td class=tdborder></td><td class=tdborder></td></tr>\n");
		}
	}
	return 0;
}

/* modified by freely@BMY@20060525 */
static int showboardlist(struct boardmem *(data[]), int total, char *secstr, const struct sectree *sec) {
	char *cgi = "home", bmbuf[IDLEN * 4 + 4], *ptr;
	int sortmode, i;
	sortmode = atoi(getparm("sortmode"));
	if (sortmode <= 0 || sortmode > 3)
		sortmode = 2;
	if (w_info->def_mode)
		cgi = "tdoc";
	switch (sortmode) {
	case 1:
		qsort(data, total, sizeof (struct boardmem *), (void *) cmpboard);
		break;
	case 2:
		qsort(data, total, sizeof (struct boardmem *), (void *) cmpboardscore);
		break;
	case 3:
		qsort(data, total, sizeof (struct boardmem *), (void *) cmpboardinboard);
		break;
	}
	printf("%s", "<tr><td width=40 class=\"level1\">&nbsp;</td>\n"
		"<td colspan=2 class=\"level1\"><TABLE width=\"90%\" border=0 cellPadding=2 cellSpacing=0>\n"
		"<TBODY>\n");

	printf("<TR>\n<TD class=tdtitle>δ</TD>\n"
		"<TD class=tduser><a href=\"boa?secstr=%s\" class=linktheme>����������</a></TD>\n"
		"<TD class=tdtitle>V\n"
		"<TD class=tdtitle>���</TD>\n"
		"<TD class=tdtitle>��������</TD>\n"
		"<TD class=tdtitle>����</TD>\n"
		"<TD class=tdtitle>������</TD>\n"
		"<TD class=tdtitle><a href=\"boa?secstr=%s&sortmode=2\" class=linktheme>����</a></TD>\n"
		"<TD class=tdtitle><a href=\"boa?secstr=%s&sortmode=3\" class=linktheme>����</a></TD>\n"
		"</TR>\n", secstr, secstr, secstr);
	brc_initial(currentuser.userid, NULL);
	printf("<tr>\n");

	/* add by freely@BMY@20060525 */
	/*��showsecintro�ŵ�showboardlist �������*/
	/*����ʾ��������*/
	if(secstr[0]!='*')
		showsecintro(sec);

	for (i = 0; i < total; i++) {
		printf("<td class=tdborder>%s</td>\n", board_read(data[i]->header.filename, data[i]->lastpost) ? "��" : "��");
		printf("<td class=tduser><a href=%s?B=%s >%s</a></td>", cgi, data[i]->header.filename, data[i]->header.filename);
		printf("<td class=tdborder>");
		if (data[i]->header.flag & VOTE_FLAG)
			printf ("<a href=vote?B=%s>V</a>", data[i]->header.filename);
		else
			printf("&nbsp;");
		printf("</td>");
		printf("<td class=tdborder>[%4.4s]</td>", data[i]->header.type);
		printf("<td class=tdborder><a href=%s?B=%s>%s</a></td>", cgi, data[i]->header.filename, data[i]->header.title);
		ptr = userid_str(bm2str(bmbuf, &(data[i]->header)));
		if (strlen(ptr) == 0)
			printf("<td class=tdborder>����������</td>");
		else
			printf("<td class=tdborder>%s</td>", ptr);
		printf("<td class=tdborder>%d</td><td class=tdborder>%d</td><td class=tdborder>%d</td></tr>\n", data[i]->total, data[i]->score, data[i]->inboard);
		printlastmark(data[i]->header.filename);
	}
	printf("</TR></TBODY></TABLE></td></tr></table></td></tr></table>\n");
	return 0;
}

int board_read(char *board, int lastpost) {
	brc_initial(NULL, board);
	return !brc_un_read_time(lastpost);
}

void printlastmark(char *board) {
	char buf[200], *title, *thread;
	FILE *fp;
	int n = 0;
	int th;
	sprintf(buf, "wwwtmp/lastmark/%s", board);
	if (!file_exist(buf))
		goto END;
	if ((fp = fopen(buf, "r")) == NULL)
		goto END;
	while (fgets(buf, 200, fp)) {
		thread = strchr(buf, '\t');
		if (thread == NULL)
			break;
		*thread = 0;
		thread++;
		th = atoi(thread);
		title = strchr(thread, '\t');
		if (title == NULL)
			break;
		*title = 0;
		title++;
		printf("<tr><td></td><td colspan=8>");
		printf("��<a href='tfind?B=%s&th=%d&T=%s' class=mar>%s</a> ����[%s]\n",
				board, th, encode_url(title),
				void1(titlestr(title)), userid_str(buf));
		printf("</td></tr>");
		n = 1;
	}
	fclose(fp);
END:
	if (!n)
		printf("<tr><td colspan=9 height=1></td></tr>");
}

static int showsecmanager(const struct sectree *sec) {
	struct secmanager *secm;
	int i;
	if (!sec->basestr[0] || !(secm = getsecm(sec->basestr)) || !secm->n)
		return -1;
	printf("����:");
	for (i = 0; i < secm->n; i++) {
		printf(" <a href=qry?U=%s>%s</a>", secm->secm[i], secm->secm[i]);
	}
	return 0;
}
int
show_commend()
{
	FILE *fp;
	struct commend x;
	char allcanre[256];
//	int no=0, end = 0;
	int i;//, total;
	fp=fopen(COMMENDFILE,"r");

//tj change here 20040421
//modify by mintbaggio 040517 for new www
	if (!fp)
		http_fatal("Ŀǰû���κ��Ƽ�����");

	fseek(fp, -20*sizeof(struct commend), SEEK_END);

	char* commends[20];
	int count=0;
	for(i=0; i<20; i++) {
		strcpy(allcanre, "");
		if(fread(&x, sizeof(struct commend), 1, fp)<=0) break;
		commends[i]=malloc(1024);
		if(x.accessed & FH_ALLREPLY)
			strcpy(allcanre," style='color:red;' ");
		sprintf(commends[i], "<tr><td></td>\n");
		char temp[1024];
		sprintf(temp, "<td><a href=con?B=%s&F=%s%s>%-30s</a> / <a href=qry?U=%s class=linkdatetheme>%-12s</a>"
			"/<a href=\"%s%s\" class=linkdatetheme>%-13s</a></td></tr>\n",
			x.board, x.filename, allcanre, x.title,x.userid,  x.userid, showByDefMode(), x.board, x.board);
		strcat(commends[i], temp);
		++count;
	}
	int index;
	for (index=count; index>count-10; --index) {
		if (index<1) break;
		printf("%s", commends[index-1]);
		free(commends[index-1]);
	}
	fclose(fp);
	return 0;
}

int show_commend2()
{
	FILE *fp;
	struct commend x;
	char allcanre[256];
	int i;//, total;
	fp=fopen(COMMENDFILE2,"r");

//tj change here 20040421
//modify by mintbaggio 040517 for new www
	if (!fp)
		http_fatal("Ŀǰû���κ�֪ͨ����");

	fseek(fp, -20 * sizeof(struct commend), SEEK_END);
	char* commends[20];
	int count=0;
	for(i=0; i<20; i++) {
		strcpy(allcanre, "");
		if(fread(&x, sizeof(struct commend), 1, fp)<=0) break;
		commends[i]=malloc(1024);
		if(x.accessed & FH_ALLREPLY)
			strcpy(allcanre," style='color:red;' ");
		sprintf(commends[i], "<tr><td></td>\n");
		char temp[1024];
		sprintf(temp, "<td><a href=con?B=%s&F=%s%s>%-30s</a> / <a href=qry?U=%s class=linkdatetheme>%-12s</a>"
			"/<a href=\"%s%s\" class=linkdatetheme>%-13s</a></td></tr>\n",
			x.board, x.filename, allcanre, x.title,x.userid,  x.userid, showByDefMode(), x.board, x.board);
		strcat(commends[i], temp);
		++count;
	}
	int index;
	for (index=count; index>count-10; --index) {
		if (index<1) break;
		printf("%s", commends[index-1]);
		free(commends[index-1]);
	}

	fclose(fp);
	return 0;
}

//add by mintbaggio 040517 for new www, modify tj's code
void show_banner() {
	FILE* fp;
	char buf[512];
	printf("%s", "<table width=\"100%\" border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td height=30></td></tr>\n"
			"<tr><td height=70>\n"
			"<table width=\"100%\" height=\"100%\" border=0 cellpadding=0 cellspacing=0 class=\"level2\">\n"
			"<tr><td><div id=\"bmy\"><span class=\"hidden\">����ٸ</span></div></td>\n");

	fp = fopen("etc/ad_banner", "r");
	if(!fp){
		goto endbanner;
	}
	bzero(buf, 512);
	while(fgets(buf, 512, fp)){
		ytht_strtrim(buf);
		if (strlen(buf) <= 1)
			continue;
		char *p = strchr(buf, ' ');
		if (p == NULL)
			continue;
		*p = '\0';

		printf("<td align=right width=\"468\" height=\"60\">\n"
				"<a href=\"%s\" target=\"_blank\">"
				"<img src=\"%s\" width=\"468\" height=\"60\" border=0>"
				"</img></a></td>\n", p+1, buf);
	}
	fclose(fp);

endbanner:
	//end ���
	printf("</td>\n"
			"<td align=right width=25>&nbsp;</td></tr></table>\n"
			"</td></tr></table>\n");
	return;
}

void title_begin(char *title)
{
	printf("<br>\n"
			"<table width=275 border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td valign=top class=bordertheme>\n"
			"<table width=143 border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td width=10 rowspan=2 align=right>\n"
			"<img src=\"/images/bmy_arrowdown_orange.gif\" width=6 height=5></td>\n"
			"<td width=32 height=5></td></tr>\n"
			"<tr><td class=themetext>%s</td></tr></table></td></tr></table>\n"
			"<table width=275 border=0 cellpadding=0 cellspacing=0 class=bordergrey2>\n"
			"<tr><td width=6 rowspan=2 class=B0010>&nbsp;</td>\n"
			"<td colspan=2 height=6></td></tr>\n"
			"<tr><td colspan=2>", title);

}

void title_end()
{
	printf("</td></tr></table>\n");
}

/* ����ٸ���� */
//add by mintbaggio 040517 for new www
int show_content()
{
	FILE* fp, *secorderfile;
	char buf[512], str[1], buf1[512], buf2[512], secorder[16];
	size_t sec_index, sec_length;
	struct sectree * psec;
	const char * secorderfilepath = BBSHOME "/etc/secorder";

	//show commend
	printf("%s", "<table width=100% border=0 cellpadding=0 cellspacing=0>\n"
			"<tr> \n<td valign=top> \n"
			"<table width=98%% border=0 align=center cellpadding=0 cellspacing=0>\n"
			"<tr> \n"
			"<td width=7 rowspan=2 align=right><img src=\"/images/bmy_arrowdown_black.gif\" width=6 height=5></td>\n"
			"<td height=5>&nbsp;</td>\n"
			"</tr>\n"
			"<tr> \n"
			"<td class=F0000>�����Ƽ� /  <a href=\"bbstop10\" target=f3 class=linkdatetheme>�鿴����</a></td>\n"
			"</tr>     \n ");
	show_commend();
	printf("<tr><td></td><td><div class=\"linediv\"></div></td></tr>");
	printf("</table>\n");

	printf("%s",
			"<table width=98%% border=0 align=center cellpadding=0 cellspacing=0>\n"
			"<tr> \n"
			"<td width=7 rowspan=2 align=right><img src=\"/images/bmy_arrowdown_black.gif\" width=6 height=5></td>\n"
			"<td height=5>&nbsp;</td>\n"
			"</tr>\n"
			"<tr> \n"
			"<td class=F0000>֪ͨ����  /  <a href=\"bbstop10\" target=f3 class=linkdatetheme>�鿴����</a></td>\n"
			"</tr>     \n ");
	show_commend2();
	printf("<tr><td></td><td><div class=\"linediv\"></div></td></tr>");
	printf("</table>\n");

	//show boards
	printf("<table width=98%% border=0 align=center cellpadding=0 cellspacing=0>\n"
		"<tr><td width=456><img src=\"/images/bmy_arrowdown_black.gif\">\n"
		"<span=2 class=F0000>�Ƽ�������</span></td>\n"
		"<td>&nbsp;</td></tr>\n"
		"<tr><td><a href='%sXJTUdevelop' style=\"color: red\">����չ</a>&nbsp;<a href='%swelcome' style=\"color: red\">��ӭ������������</a>&nbsp;<a href='%skaoyan' style=\"color: red\">�����뱣��</a></td></tr>\n",showByDefMode(),showByDefMode(),showByDefMode());

	//show_sec(&sectree); �ϰ汾����ʾ�����ķ�ʽ��ע�͵� by IronBlood@bmy 20120329

	memset(secorder,0,sizeof(secorder));

	if( access(secorderfilepath,F_OK) != -1 ){
		secorderfile = fopen(secorderfilepath, "r");
		while(fgets(secorder,sizeof(secorder),secorderfile)!=NULL){
			if (secorder[strlen(secorder) - 1] == '\n')
				secorder[strlen(secorder) - 1] = 0;
		}
		fclose(secorderfile);
	}
	else{
		strcpy(secorder,"0123456789GNHAC"); // ���վ��û�����ã��ǾͰ����ϰ汾����
	}

	for(sec_index=0, sec_length = strlen(secorder); sec_index!=sec_length; ++sec_index){
		show_sec_by_name(secorder[sec_index]);
	}


	//show right top header
	printf("</table>\n<br></td>\n");
	printf("<td width=15></td>\n<td width=280 valign=top>\n");

	//add by macintosh 070529 for board searching by keywords
	printf("<table width=100%% border=0 cellpadding=0 cellspacing=0>\n"
	"<tr><td width=290><table border=0 cellpadding=0 cellspacing=0>\n"
	"<tr><form action=bbssbs target=f3><td>\n"
	"<div title=\"֧����Ӣ�İ���/����ؼ��ֶ�λ�����档\n���磬���롰��·���ɶ�λ��traffic�档\">"
	"<input type=text name=keyword maxlength=25 size=25 onclick=\"this.select()\" value=\"������ؼ���\">\n"
	"<input type=submit class=sumbitgrey value=\"��������\"></div>\n"
	"</td></form></tr></tabel></td></tr>\n"
	"</table><br>");

	//����ʮ��
	printf("<table width=98%% border=0 cellpadding=0 cellspacing=0>\n"
		"<tr>\n<td width=10 rowspan=2 align=right><img src=\"/images/bmy_arrowdown_black.gif\" width=6 height=5></td>\n"
		"<td width=456 height=5>&nbsp;</td>\n"
		"</tr><tr>\n<td>����ʮ��&nbsp;<a href=\"http://" MY_BBS_DOMAIN "/" SMAGIC "/rss?board=TOP10\" target=\"blank\"><img  src=\"/images/rss.gif\" border=\"0\" /></a></td></tr></table>\n");

	//show top10
	show_right_click_header(1);
	//content
	printf("<table width=275 border=0 cellpadding=0 cellspacing=0>\n");
	show_top10();
	printf("</table></div>\n");

	//show link
	show_right_click_header(2);
	//content
	printf("<table width=275 border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td><a href=\"http://www.xjtu.edu.cn/\">������ҳ</a></td></tr>\n"
			"<tr><td><a href=\"http://ic.xjtu.edu.cn/\">��Ϣ����</a></td></tr>\n"
			"<tr><td><a href=\"http://nic.xjtu.edu.cn/\">��������</a></td></tr>\n"
			"<tr><td><a href=\"http://ftp.xjtu.edu.cn\">˼ԴFTP </a></td></tr>\n"
			"<tr><td><a href=\"http://webmail.xjtu.edu.cn\">˼ԴWEBMAIL </a></td></tr>\n"
			"<tr><td><a href=\"https://stu.xjtu.edu.cn\">˼Դѧ��MAIL </a></td></tr>\n"
			"<tr><td><a href=\"http://music.xjtu.edu.cn/\">˼Դ����̨</a></td></tr>\n"
			"<tr><td><a href=\"http://vod.xjtu.edu.cn/\">˼ԴVOD </a></td></tr>\n"
			"<tr><td><a href=\"http://202.117.21.253/\">Windows Update </a></td></tr>\n"
			"<tr><td>&nbsp;</td></tr>\n"
			"</table></div>\n");

	//show other
	show_right_click_header(3);
	//content
	printf("<table width=275 border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td><a href=\"telnet://bbs.xjtu.edu.cn\">Telnet��¼BMY</a></td></tr>\n"
			"<tr><td><a href=\"javascript:window.external.AddFavorite('http://bbs.xjtu.edu.cn/','������ͨ��ѧ����ٸBBS')\">����վ�����ղؼ�</a></td>\n"
			"</tr>\n"
			"<tr><td><a href=\"mailto:wwwadmin@mail.xjtu.edu.cn\">��ϵվ���� </a></td></tr>\n"
			"<tr><td><a href=\"javascript: openreg()\">���û�ע�� </a></td></tr>\n"
			"</table></div>\n");

	title_begin("���������Ϣ");
	fp = fopen("etc/adpost", "r");
	if(!fp){
		//printf("fail to open\n");
		goto newboard;
	}
	bzero(buf1, 512);
	bzero(buf2, 512);
	printf("<marquee scrollamount=1 scrolldelay=20 direction= UP width=200 height=80  onmouseover=\"this.stop();\" onmouseout=\"this.start();\">\n");
	while(fgets(buf1, 512, fp)){
		ytht_strtrim(buf1);
		if (strlen(buf1) <= 1)
			continue;

		char *p = strchr(buf1, ' ');
		if (p == NULL)
			continue;
		*p = '\0';

		strcpy(buf2, p+1);
		ytht_strtrim(buf2);
		if (strlen(buf2) <= 1)
			continue;
		p = strchr(buf2, ' ');
		if (p == NULL)
			continue;
		*p = '\0';

		printf("<a href=\"con?B=%s&F=%s\">%s</a><br>\n", buf1,buf2, p+1);
	}
	fclose(fp);
	printf("</marquee>\n");;


newboard:
	// 20121016 move this line from 2lines up; so that it can't be skipped BY liuche
	// and i do the same thing to every "title_end();"
	title_end();
	title_begin("�¿�����");
	fp = fopen("etc/newboard", "r");
	if(!fp){
		//printf("fail to open newboard\n");
		goto recommboard;
	}
	bzero(buf, 512);
	while(fgets(buf, 512, fp)){
		ytht_strtrim(buf);
		if (strlen(buf) <= 1)
			continue;
		char *p = strchr(buf, ' ');
		if (p == NULL)
			continue;
		*p = '\0';
		printf("<a href=\"%s%s\">%s</a><br>\n", showByDefMode(), buf, p+1);
	}
	fclose(fp);


recommboard:
	title_end();
	title_begin("�Ƽ�����");
	fp = fopen("etc/recommboard", "r");
	if(!fp){
		//printf("fail to open commboard\n");
		goto aboutbmy;
	}
	bzero(buf, 512);
	while(fgets(buf, 512, fp)){
		ytht_strtrim(buf);
		if (strlen(buf) <= 1)
			continue;
		char *p = strchr(buf, ' ');
		if (p == NULL)
			continue;
		*p = '\0';
		printf("<a href=\"%s%s\">%s</a><br>\n", showByDefMode(), buf, p+1);
	}
	fclose(fp);

/*
	title_begin("telnet��������");
	printf("ctrl+g���Ը����������ݡ��� �ߡ�����ȷ���������Tab �鿴����¼��z �鿴���ܱ���¼��x ���뾫������h �鿴 һ���԰����˵����༭ʱctrl+Q�鿴�����˵�");
	title_end();
*/
aboutbmy:
	title_end();
	title_begin("����BMY");
	printf("CPU: Intel<sup>&reg;</sup> Xeon<sup>&reg;</sup> E5-2620 2.1GHz ��4<br>RAM: 16GB ECC<br>HD: SAN 4000G<br>\n"
			"����: ˫1000Mbps NIC<br>\n");
	title_end();

	// management team
	title_begin("�����Ŷ�");

	fp = fopen("etc/manager_team", "r");

	if(!fp){
		printf("fail to open team\n");
		goto fail_out;
	}
	bzero(buf, 512);
	while(fgets(buf, 512, fp)){
		buf[strlen(buf)-1] = 0;
		printf("%s<br>\n", buf);
	}
	fclose(fp);
fail_out:

	/*if(show_manager_team())
		printf("fail to open team\n");

	*/
	title_end();
	printf("<br></div><br><br>"
	"</td></tr></table>\n");
	//fflush(NULL);
	return 1;
}

void show_sec_by_name(char secid){
	const struct sectree *sec;
	char str_secid[2];
	sprintf(str_secid, "%c", secid);
	sec = getsectree(str_secid);
	printf("<tr>");
	printf("<td><div class=\"linediv\"><a href=boa?secstr=%s class=linkboardtheme>"
			"%s</a></div></td>\n", sec->basestr, nohtml(sec->title));
	printf("<td rowspan=2 align=right valign=bottom width=45><a href=boa?secstr=%s class=linkbigtheme>%s</a></td></tr>\n", sec->basestr, sec->basestr);
	show_boards(sec->basestr);
	printf("</td></tr>\n");
}

//add by mintbaggio 040517 for new www
void show_sec(const struct sectree *sec) {
	int i;
	for (i = 0; i < sec->nsubsec; i++) {
		printf("<tr>");
		printf("<td><div class=\"linediv\"><a href=boa?secstr=%s class=linkboardtheme>"
				"%s</a></div></td>\n", sec->subsec[i]->basestr,
				nohtml(sec->subsec[i]->title));
		//modified by safari 20091231
		printf("<td rowspan=2 align=right valign=bottom width=45><a href=boa?secstr=%s class=linkbigtheme>%s</a></td></tr>\n",
			sec->subsec[i]->basestr, sec->subsec[i]->basestr);
		show_boards(sec->subsec[i]->basestr);

		printf("</td></tr>\n");
	}
	return;
}

//add by mintbaggio 040518 for new www
void show_boards(const char *secstr) {
	struct boardmem *(data[MAXBOARD]);
	int hasintro = 0;
	int i, total = 0;
	const struct sectree *sec;

	sec = getsectree(secstr);
	if (sec->introstr[0])
		hasintro = 1;

	ythtbbs_cache_Board_foreach_v(filter_board_v, FILTER_BOARD_with_secnum, data, &total, hasintro, secstr);

	show_sec_boards(data, total);
}

//add by mintbaggio 040518 for new www
void show_sec_boards(struct boardmem *(data[]), int total) {
	int i;
	int scores[total];
	scores[0] = 0;
	for (i = 1; i < total; i++) {
		int j, k;
		for (j = 0; j < i; j++)
			if (data[i]->score > data[scores[j]]->score)
				break;
		for (k = i - 1; k >= j; k--)
			scores[k+1] = scores[k];
		scores[j] = i;
	}
	printf("<tr><td>\n");
	if (total < SHOWBOARDS) {
		for (i = 0; i < total; i++)
			printf("<a href=%s%s>%s</a>(%d) ", showByDefMode(), data[scores[i]]->header.filename, data[scores[i]]->header.title, data[scores[i]]->score);
	} else {
		for(i = 0; i < SHOWBOARDS; i++)
			printf("<a href=%s%s>%s</a>(%d) ", showByDefMode(), data[scores[i]]->header.filename, data[scores[i]]->header.title, data[scores[i]]->score);
		printf("<a href=boa?secstr=%c> [�������...]</a>", data[0]->header.secnumber1);
	}

	if (total == 0)
		return;
	show_area_top(data[0]->header.secnumber1);
}

void show_top10()
{
	struct mmapfile mf = {.ptr = NULL};
	check_msg();
	MMAP_TRY {
		if (mmapfile("wwwtmp/cindextopten", &mf) < 0) {
			MMAP_UNTRY;
			http_fatal("�ļ�����");
		}
		fwrite(mf.ptr, mf.size, 1, stdout);
	}
	MMAP_CATCH {
	}
	MMAP_END mmapfile(NULL, &mf);
}

void show_area_top(char c)
{
	struct mmapfile mf = {.ptr = NULL};
	char path[256];

	sprintf(path, AREA_DIR "/%c", c);
	check_msg();
	MMAP_TRY {
		if (mmapfile(path, &mf) < 0) {
			MMAP_UNTRY;
			http_fatal("�ļ�����");
		}
		fwrite(mf.ptr, mf.size, 1, stdout);
	}
	MMAP_CATCH {
	}
	MMAP_END mmapfile(NULL, &mf);
}

//add by mintbaggio 041225 for new www, need modify: i==1, i==2 else�����printf��ʮ�ֲ����
//modified by safari 091228
void show_right_click_header(int i) {
	if(i == 1)
		printf("<!-- begin:ʮ���� -->\n<div id=layer%d style=\"display:\">\n", i);
	else
		printf("<!-- begin:ʮ���� -->\n<div id=layer%d style=\"display:none\">\n", i);
	printf("<!-- begin:������ -->\n<table><tr>\n");
	if(i == 1)
		printf("<td><DIV class=\"btncurrent\" title=\"���� accesskey: t\" accesskey=\"t\">���� / topic</DIV></td>\n"
				"<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('2')\">���� / link</A></DIV></td>\n"
				"<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('3')\">���� / other</A></DIV></td>\n");
	else if(i == 2)
		printf("<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('1')\">���� / topic</A></DIV></td>\n"
			"<td><DIV class=\"btncurrent\">���� / link</DIV></td>\n"
			"<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('3')\">���� / other</A></DIV></td>\n");
	else
		printf("<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('1')\">���� / topic</A></DIV></td>\n"
			"<td><DIV><A class=\"btnlinktheme\" href=\"javascript:;\" onClick=\"Tog('2')\">���� / link</A></DIV></td>\n"
			"<td><DIV class=\"btncurrent\">���� / other</DIV></td>\n");
	printf("</tr></table>\n<!-- end:������ -->\n");
}

