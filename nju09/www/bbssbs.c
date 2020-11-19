#include "bbslib.h"

// bbsboa
int board_read(char *board, int lastpost);
void printlastmark(char *board);
// bbscon
int fshowcon(FILE * output, char *filename, int show_iframe);

static int bbssbs_callback(struct boardmem *board, int curr_idx, va_list ap) {
	const char *keyword = va_arg(ap, const char *);
	int *result = va_arg(ap, int *);
	int *total  = va_arg(ap, int *);

	if (board->header.filename[0] == '\0')
		return 0;

	if (!has_read_perm_x(&currentuser, board))
		return 0;

	if (strcasestr(board->header.filename, keyword)
			|| strcasestr(board->header.keyword, keyword)
			|| strcasestr(board->header.title, keyword)) {
		result[*total] = curr_idx;
		*total = *total + 1;
	}
	return 0;
}

int
bbssbs_main()
{
	int result[MAXBOARD], i, total=0;
	char keyword[80], *ptr, buf[128], bmbuf[(IDLEN + 1) * 4];
	struct boardmem *bp;

	html_header(1);
	check_msg();
	changemode(READING);
	ytht_strsncpy(keyword, getparm("keyword"), 32);
	printf("<body><center>");
	printf("<div class=rhead>%s -- ��������ѡ��</div><hr>\n", BBSNAME);

	if (strlen(ytht_strtrim(keyword))) {
		ythtbbs_cache_Board_foreach_v(bbssbs_callback, keyword, result, &total);
		if (total == 0){
			printf("Sorry������İ������ˣ�û�ҵ���������������������");
			printf("<p><a href=javascript:history.go(-1)>���ٷ���</a>");
			http_quit();
		}else {
			printf("<table width=\"100%%\" border=0 cellpadding=0 cellspacing=0><tr>\n"
				"<td width=40>&nbsp;</td>\n"
				"<td colspan=2 class=\"level1\"><TABLE width=\"90%%\" border=0 cellPadding=2 cellSpacing=0>\n"
				"<TBODY>\n");
			printf("<TR>\n"
				"<TD class=tdtitle>δ</TD>\n"
				"<TD class=tduser>����������</TD>\n"
				"<TD class=tdtitle>V</TD>\n"
				"<TD class=tdtitle>���</TD>\n"
				"<TD class=tdtitle>��������</TD>\n"
				"<TD class=tdtitle>����</TD>\n"
				"<TD class=tdtitle>������</TD>\n"
				"<TD class=tdtitle>����</TD>\n"
				"<TD class=tdtitle>����</TD>\n"
				"</TR>\n");
			printf("<tr>\n");
			for (i = 0; i < total; i++){
				bp = ythtbbs_cache_Board_get_board_by_idx(result[i]);
				printf("<td class=tdborder>%s</td>\n",
					board_read(bp->header.filename, bp->lastpost) ? "��" : "��");
				printf("<td class=tduser><a href=%s?B=%s >%s</a></td>\n",
					"bbsdoc", bp->header.filename, bp->header.filename);
				printf("<td class=tdborder>");
				if (bp->header.flag & VOTE_FLAG)
					printf("<a href=vote?B=%s>V</a>", bp->header.filename);
				else
					printf("&nbsp;");
				printf("</td>\n");
				printf("<td class=tdborder>[%4.4s]</td>\n", bp->header.type);
				printf("<td class=tdborder><a href=%s?B=%s>%s</a></td>\n",
					"bbshome",bp->header.filename, bp->header.title);
				ptr = userid_str(bm2str(bmbuf, &(bp->header)));
				if (strlen(ptr) == 0)
					printf("<td class=tdborder>����������</td>\n");
				else
					printf("<td class=tdborder>%s</td>\n", ptr);
				printf("<td class=tdborder>%d</td>\n"
					"<td class=tdborder>%d</td>\n"
					"<td class=tdborder>%d</td>\n</tr>\n",
					bp->total, bp->score, bp->inboard);
				setbfile(buf, bp->header.filename, "introduction");
				if (file_exist(buf)) {
					printf("<tr><td>&nbsp;&nbsp;</td>\n"
						"<td class=tduser>������: </td>\n"
						"<td class=tdborder colspan=7>"
						"<div title=\"����ؼ���: %s\">\n",
						bp->header.keyword[0] ? bp->header.keyword: "(����)");
					fshowcon(stdout, buf, 2);
					printf("</div></td></tr>\n");
				}
				printlastmark(bp->header.filename);
			}
			printf("</TR></TBODY></TABLE></td></tr></table>\n");
		}
	}
	printf("<br><form action=bbssbs>\n");
	printf("<div title=\"֧����Ӣ�İ���/����ؼ��ֶ�λ�����档���磬���롰��·���ɶ�λ��traffic�档\">"
			"<input name=keyword maxlength=20 size=20>\n");
	printf("<input type=submit value=��ʼ����></div>\n");
	printf("</form><hr>\n");
	http_quit();
	return 0;
}

