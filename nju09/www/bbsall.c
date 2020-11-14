#include "bbslib.h"

int bbsall_main() {
	//modify by mintbaggio 20040829 for new www
	struct boardmem *(data[MAXBOARD]), *x;
	int sortmode, i, total = 0;
	char bmbuf[IDLEN * 4 + 4];
	html_header(1);
	changemode(SELECT);
	check_msg();
	sortmode = atoi(getparm("sortmode"));
	if (sortmode <= 0 || sortmode > 3)
		sortmode = 2;
	printf("<style type=text/css>A {color: #0000f0}</style>");
	ythtbbs_cache_Board_foreach_v(filter_board_v, FILTER_BOARD_basic, data, &total);
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
	printf("<body><center>\n");
	printf("<div class=rhead>%s -- ȫ�������� [��������: <span class=h11>%d</span>]</div><hr>", BBSNAME, total);
	printf("<table>\n");
	printf("<tr><td>���</td><td><a href=bbsall?sortmode=1>����������</a></td><td>V</td><td><nobr>&nbsp;���&nbsp;</nobr></td><td>��������</td><td>����</td><td><a href=bbsall?sortmode=2>����</td><td><a href=bbsall?sortmode=3>����</td></tr>\n");
	for (i = 0; i < total; i++) {
		printf("<tr><td>%d</td>", i + 1);
		printf("<td><a href=%s%s>%s</a></td>",
				showByDefMode(), data[i]->header.filename, data[i]->header.filename);
		printf("<td>");
		if (data[i]->header.flag & VOTE_FLAG)
			printf("<font color=red>V</font>");
		printf("</td>");
		printf("<td>[%4.4s]</td>", data[i]->header.type);
		printf("<td><a href=%s%s>%s</a></td>",
				showByDefMode(), data[i]->header.filename, data[i]->header.title);
		bm2str(bmbuf, &(data[i]->header));
		if (strlen(bmbuf) == 0)
			printf("<td>����������</td>");
		else
			printf("<td>%s</td>", userid_str(bmbuf));
		printf("<td>%d</td>", data[i]->score);
		printf("<td>%d</td>", data[i]->inboard);
		printf("</tr>");

	}
	printf("</table><hr></center></body>\n");
	http_quit();
	return 0;
}

