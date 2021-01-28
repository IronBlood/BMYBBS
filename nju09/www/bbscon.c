#include "bbslib.h"
#include "bmy/convcode.h"

extern char *cginame;

int testmozilla(void);
extern char *get_mime_type(const char *name);

int showbinaryattach(char *filename) {
	char *attachname;
	int pos_i;
	size_t pos;
	unsigned int size;
	struct mmapfile mf = { .ptr = NULL };
//      no_outcache();
	if (cache_header(file_time(filename), 86400))
		return 0;
	attachname = getparm("attachname");
	pos_i = atoi(getparm("attachpos"));
	if (pos_i < 0) {
		http_fatal("�޷��򿪸���");
		return -1;
	}

	pos = pos_i;
	MMAP_TRY {
		if (mmapfile(filename, &mf) < 0) {
			MMAP_UNTRY;
			http_fatal("�޷��򿪸���");
			MMAP_RETURN(-1);
		}
		if (pos >= mf.size - 4 || pos < 1) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�޷��򿪸���");
			MMAP_RETURN(-1);
		}
		if (mf.ptr[pos - 1] != 0) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�޷��򿪸���");
			MMAP_RETURN(-1);
		}
		size = ntohl(*(unsigned int *) (mf.ptr + pos));
		if (pos + 4 + size >= mf.size) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�޷��򿪸���");
			MMAP_RETURN(-1);
		}
		printf("Content-type: %s\n\n", get_mime_type(attachname));
//      printf("Content-Length: %d\n\n", size);
		fwrite(mf.ptr + pos + 4, 1, size, stdout);
	}
	MMAP_CATCH {
	}
	MMAP_END mmapfile(NULL, &mf);
	return 0;
}

void fprintbinaryattachlink(FILE * fp, int ano, char *attachname, int pos, int size, char *alt, char *alt1) {
	char *ext, link[256], *ptr, *board;
	int pic = 0;
	int atthttp = 0;
	struct boardmem *x;

	//check read_perm for guest, refer to has_read_perm()
	board = getparm2("B", "board");
	x = getboard(board);
	if (x && !x->header.clubnum && !x->header.level) {
		ptr = "/" SMAGIC "/";
		if (w_info->att_mode == 0)
			atthttp = 1;
	} else {
		ptr = "";
	}

	if (alt) {
		//�������Ŀǰ������atthttpd
		if (!atthttp)
			snprintf(link, sizeof (link),
				"%s%s&attachpos=%d&attachname=/%s",
				ptr, alt, pos, attachname);
		else
			snprintf(link, sizeof (link),"/attach/%s/%d/%s", alt1, pos, attachname);

	} else if (!strcmp(cginame, "bbscon")) {
		//ͬ��
		if (!atthttp)
			snprintf(link, sizeof (link),
				"%sattach/bbscon/%s?B=%s&F=%s&attachpos=%d&attachname=/%s",
				ptr, attachname, board, getparm2("F", "file"),
				pos, attachname);
		else
			snprintf(link, sizeof (link),"/attach/%s/%s/%d/%s", board, getparm2("F", "file"), pos, attachname);
	} else
		snprintf(link, sizeof (link),
			"attach/%s/%s?%s&attachpos=%d&attachname=/%s",
			cginame, attachname, getsenv("QUERY_STRING"), pos,
			attachname);

	if ((ext = strrchr(attachname, '.')) != NULL) {
		if (!strcasecmp(ext, ".bmp") || !strcasecmp(ext, ".jpg")
				|| !strcasecmp(ext, ".gif")
				|| !strcasecmp(ext, ".jpeg")
				|| !strcasecmp(ext, ".png")
				|| !strcasecmp(ext, ".pcx"))
			pic = 1;
		else if (!strcasecmp(ext, ".mp4"))
			pic = 2;
		else
			pic = 0;
	}
	switch (pic) {
	case 1:
		fprintf(fp, "%d ��ͼ: <a href='%s'>%s</a> (%d �ֽ�)<br>"
			"<IMG style=\" max-width:800px; width: expression(this.width > 800 ? 800: true); height:auto\" SRC='%s'  border=0/></a>",
			ano, link, attachname, size, link);
		break;
	case 2:
		fprintf(fp, "%d ��Ƶ: <a href='%s'>%s</a> (%d �ֽ�)<br>"
			"<video controls src='%s'>",
			ano, link, attachname, size, link);
		break;
	default:
		fprintf(fp, "%d ����: <a href='%s'>%s</a> (%d �ֽ�)",
			ano, link, attachname, size);
	}
}

/* show_iframe: 0  just as normail
		1  just show iframe
		2  show the content of iframe
*/
int
fshowcon(FILE * output, char *filename, int show_iframe)
{
	char *ptr, buf[512];
	FILE *fp;
	int lastq = 0, ano = 0, in_sig = 0;
	if (show_iframe != 2) {
//		fprintf(output, "<table width=100%% border=1><tr>");
		fprintf(output, "<tr><td width=40 class=\"level1\">&nbsp;</td>\n<td class=\"level1\"><br><TABLE width=\"95%%\" cellpadding=5 cellspacing=0><TBODY>\n<tr><td class=tdtitletheme>&nbsp;</td></tr><tr>\n");
		fprintf(output, "<td class=\"bordertheme\">\n");
	}
	fp = fopen(filename, "r");
	if (fp == 0)
		return -1;
	fdisplay_attach(NULL, NULL, NULL, NULL);
	printf("<div id='filecontent' style='width:800px;'>\n");
	while (1) {
		if (fgets(buf, sizeof (buf), fp) == 0)
			break;
		if (!strncmp(buf, "begin 644 ", 10)) {
			ano++;
			ptr = strrchr(filename, '/');
			errlog("old attach %s", filename);
			fdisplay_attach(output, fp, buf, ptr + 1);
			fprintf(output, "\n<br>");
			continue;
		}
		if (!strncmp(buf, "beginbinaryattach ", 18)) {
			unsigned int len;
			char ch;
			fread(&ch, 1, 1, fp);
			if (ch != 0) {
				ungetc(ch, fp);
				fhhprintf(output, "%s", buf);
				continue;
			}
			ptr = strchr(buf, '\r');
			if (ptr)
				*ptr = 0;
			ptr = strchr(buf, '\n');
			if (ptr)
				*ptr = 0;
			ano++;
			ptr = buf + 18;
			fread(&len, 4, 1, fp);
			len = ntohl(len);
			fprintbinaryattachlink(output, ano, ptr, -4 + (int) ftell(fp), len, NULL, NULL);
			fseek(fp, len, SEEK_CUR);
			continue;
		}
		if (buf[0] == ':' && buf[1] == ' ') {
			if (!lastq)
				fprintf(output, "<font color=808080>");
			lastq = 1;
		} else {
			if (lastq)
				fprintf(output, "</font>");
			lastq = 0;
		}

		if (!strncmp(buf, "--\n", 3)) {
			if (!in_sig) {
				fprintf(output, "<div class=\"con_sig\">");
				in_sig = 1;
			}
		}
		fhhprintf(output, "%s", buf);
	}
	printf("</div>\n");
	if (lastq)
		fprintf(output, "</font>");
	if (in_sig)
		fprintf(output, "</div>");
	fclose(fp);
	if (show_iframe != 2)
		fprintf(output, "\n</td></TR></TBODY></TABLE><br></td></tr>\n");
	return 0;
}

int
showcon(char *filename)
{
	return fshowcon(stdout, filename, 0);
}

int
showconxml(char *filename, int viewertype)
{
	char filetmp[200], cmd[512], *ptr, *pend;
	struct mmapfile mf = { .ptr = NULL };
	FILE *fp;
	int retv = -1;
	if (viewertype != 1)
		printf("<br>����ʹ����<a href=home/boards/BBSHelp/html/itex/itexintro.html target=_blank>Tex��ѧ��ʽ</a><br>");
	fp = fopen("bbstmpfs/tmp/testxml.txt", "w");
	fshowcon(fp, filename, 0);
	fclose(fp);
	sprintf(filetmp, "bbstmpfs/tmp/xml.%d.tmp", thispid);
	if (viewertype != 1)
		sprintf(cmd, MY_BBS_HOME "/bin/tidy -iso2022 -f /dev/null | "
			MY_BBS_HOME "/bin/itex2MML |" MY_BBS_HOME
			"/bin/mathml4mathplayer > %s", filetmp);
	else
		sprintf(cmd,
			MY_BBS_HOME "/bin/tidy -iso2022 -f /dev/null | "
			MY_BBS_HOME
			"/bin/itex2MML | sed -es/'<br>'/'<br \\/>'/g > %s",
			filetmp);
	fp = popen(cmd, "w");
	if (!fp)
		return -1;
	fshowcon(fp, filename, 0);
	pclose(fp);
	MMAP_TRY {
		if (mmapfile(filetmp, &mf) < 0) {
			unlink(filetmp);
			MMAP_RETURN(-1);
		}
		ptr = ytht_strncasestr(mf.ptr, "<body>", mf.size);
		if (!ptr) {
			mmapfile(NULL, &mf);
			unlink(filetmp);
			MMAP_RETURN(retv);
		}
		ptr += 6;
		pend = ytht_strncasestr(ptr, "</body>", mf.size - (ptr - mf.ptr));
		if (!pend)
			pend = mf.ptr + mf.size;
		fwrite(ptr, pend - ptr, 1, stdout);
		if (ytht_strncasestr(ptr, "</table>", pend - ptr) == NULL)
			printf("</td></tr></table>");
	}
	MMAP_CATCH {
	}
	MMAP_END mmapfile(NULL, &mf);
	unlink(filetmp);
	return retv;
}

int
showcon_cache(char *filename, int level, int edittime)
{
	char showfile[NAME_MAX + 1];
	static char showpath[PATH_MAX + 1];
	struct stat st;
	char *ptr = NULL;
	FILE *fp;
	int fd;
	if (!level)
		return fshowcon(stdout, filename, 0);
	if (!edittime)
		strncpy(showfile, filename, NAME_MAX);
	else
		snprintf(showfile, NAME_MAX, "%s:%d", filename, edittime);
	ytht_normalize(showfile);
	snprintf(showpath, PATH_MAX, "%s/%s", ATTACHCACHE, showfile);
	if (access(showpath, R_OK)) {
		if (level < 2)
			return fshowcon(stdout, filename, 0);
		fp = fopen(showpath, "w");
		if (!fp)
			return fshowcon(stdout, filename, 0);
		fshowcon(fp, filename, 0);
		fclose(fp);
		//printf("<!--make cache-->");
	}
	//printf("<!--show cache-->");

	fd = open(showpath, O_RDONLY);
	if (fd < 0) {
		fshowcon(stdout, filename, 0);
		return -1;
	}
	if (fstat(fd, &st) < 0 || !S_ISREG(st.st_mode) || st.st_size <= 0) {
		close(fd);
		fshowcon(stdout, filename, 0);
		return -1;
	}
	MMAP_TRY {
		ptr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
		close(fd);
		if (ptr == NULL) {
			fshowcon(stdout, filename, 0);
			MMAP_RETURN(-1);
		}
		fwrite(ptr, st.st_size, 1, stdout);
	}
	MMAP_CATCH {
		close(fd);
	}
	MMAP_END munmap(ptr, st.st_size);
	return 0;
}

int testmozilla() {
	char *ptr = getsenv("HTTP_USER_AGENT");
	if (strcasestr(ptr, "Mozilla") && !strcasestr(ptr, "compatible"))
		return 1;
	return 0;
}

int
testxml()
{
	char *ptr = getsenv("HTTP_USER_AGENT");
	if (strcasestr(ptr, "Mozilla/5") && !strcasestr(ptr, "compatible"))
		return 1;
	if (strcasestr(ptr, "MSIE 6.") || strcasestr(ptr, "MSIE 5.5"))
		return 2;
	return 0;
}

void
processMath()
{	//add by macintosh 050619 for Tex Math Equ
	if(usedMath) {
		printf("<script src=/jsMath/jsMath.js></script>");
		printf("<script>jsMath.ProcessBeforeShowing();</script>");
	}
}


int
bbscon_main()
{	//modify by mintbaggio 050526 for new www
	char board[32 /* max 24 */], dir[80], file[32], filename[80], fileback[128], *ptr;
	char buf[2048];
	char bmbuf[IDLEN * 4 + 4];
	int thread;
	int nbuf = 0, usexml = 0;
	struct fileheader *x = NULL, *dirinfo = NULL;
	struct boardmem *bx;
	int num, total, sametitle;
	int prenum, nextnum;
	int outgoing;
	int inndboard;
	struct mmapfile mf = { .ptr = NULL };
	char title_utf8[480];
	memset(title_utf8, '\0', sizeof(title_utf8));

	changemode(READING);

	ytht_strsncpy(board, getparm("B"), sizeof(board));
	if (!board[0])
		ytht_strsncpy(board, getparm("board"), sizeof(board));
	ytht_strsncpy(file, getparm("F"), sizeof(file));
	if (!file[0])
		ytht_strsncpy(file, getparm("file"), sizeof(file));
	num = atoi((ptr = getparm("N"))) - 1;
	if (!ptr[0])
		num = atoi(getparm("num")) - 1;
	sametitle = atoi(getparm("st"));

	if ((bx = getboard(board)) == NULL)
		http_fatal("�����������");
	if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2) && strncmp(file, "T.", 2))	//modify by mintbaggio 040517 for new www
		http_fatal("����Ĳ���1");
	if (strstr(file, "..") || strstr(file, "/"))
		http_fatal("����Ĳ���2");
	sprintf(filename, "boards/%s/%s", board, file);
	if (*getparm("attachname") == '/') {
		showbinaryattach(filename);
		return 0;
	}

	// ɾ���ظ����ѿ�ʼ by IronBlood
	time_t article_id = fn2timestamp(file);
	if(is_post_in_notification(currentuser.userid, board, article_id)) {
		del_post_notification(currentuser.userid, board, article_id);
	}
	// ɾ���ظ����ѽ���

	strcpy(fileback, file);
	sprintf(dir, "boards/%s/.DIR", board);
	total = bx->total;
	inndboard = bx->header.flag & INNBBSD_FLAG;
	if (total <= 0)
		http_fatal("�������������ڻ���Ϊ��");
	MMAP_TRY {
		if (mmapfile(dir, &mf) == -1) {
			MMAP_UNTRY;
			http_fatal("�������������ڻ���Ϊ��");
		}
		dirinfo = findbarticle(&mf, file, &num, 1);
		if (dirinfo == NULL) {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
		}

#if 1
		html_header(1);
		if (dirinfo->accessed & FH_MATH) {
			usingMath = 1;
			usedMath = 1;
			withinMath = 0;
		} else {
			usingMath = 0;
		}
#else
		if (dirinfo->accessed & FH_MATH && (usexml = testxml())) {
			html_header(100 + usexml - 1);
		} else {
			usexml = 0;
			html_header(1);
		}
#endif
		check_msg();
		// output post title and link by IronBlood@bmy 2011.12.06
		x = (struct fileheader *)(mf.ptr + num * sizeof (struct fileheader));
		g2u(x->title, strlen(x->title), title_utf8, sizeof(title_utf8));
		printf("<title>%s | ����ٸBBS</title>", x->title);

		printf("<script src='/function.js'></script></head>\n");

		printf("<body leftmargin=0 topmargin=0>\n<img src=\"/images/bmy.gif\" style=\"position: absolute;top:-160px;\"/>\n");
		printf("%s", "<table width=\"100%\" border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td height=30 colspan=2> \n"
			"<table width=\"100%\"  border=0 cellspacing=0 cellpadding=0>\n"
			"<tr><td width=40><img src=\"/images/spacer.gif\" width=40 height=10 alt=\"\"></td>\n"
			"<td><table width=\"100%\" border=0 align=right cellpadding=0 cellspacing=0>\n");
		if (loginok && !isguest && (dirinfo->accessed & FH_ATTACHED))
			printf("<a href=bbsmywww><font color=red>������ͼƬ��</font></a>");

		printf("<tr><td><a href=\"boa?secstr=%s\">%s</a> / <a href=\"%s%s\">%s</a> / �Ķ����� "
			"</td></tr></table></td>\n", bx->header.sec1, nohtml(getsectree(bx->header.sec1)->title), showByDefMode(), board, board);
		printf("<td><table border=0 align=right cellpadding=0 cellspacing=0>\n"
			"<tr><td> ���� %s</tr></table></td></tr></table></td></tr>\n",
			userid_str(bm2str(bmbuf, &(bx->header))));
		if (dirinfo->accessed & FH_ALLREPLY) {
			FILE *fp;
			fp = fopen("bbstmpfs/dynamic/Bvisit_log", "a");
			if (NULL != fp) {
				fprintf(fp,
					"www user %s from %s visit %s %s %s",
					currentuser.userid, fromhost,
					bx->header.filename, fh2fname(dirinfo),
					ctime(&now_t));
				fclose(fp);
			}
		}
		if (dirinfo->owner[0] == '-') {
			mmapfile(NULL, &mf);
			MMAP_UNTRY;
			http_fatal("�����ѱ�ɾ��");
		}
		thread = dirinfo->thread;
		printf("<tr><td height=70 colspan=2>\n"
			"<table width=\"100%%\" height=\"100%%\" border=0 cellpadding=0 cellspacing=0 bgcolor=#efefef>\n"
			"<tr><td width=40>&nbsp; </td>\n"
			"<td height=70>\n"
			"<table width=\"95%%\" height=\"100%%\"  border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td colspan=2 valign=bottom>\n"
			"<table width=\"100%%\" border=0 cellpadding=0 cellspacing=0>\n");
		nbuf = sprintf(buf, "<tr><td><div class=\"menu\">\n<DIV class=btncurrent>&lt;%s&gt;</DIV>\n", void1(titlestr(bx->header.title)));
		nbuf += sprintf(buf+nbuf,
			"<A href='fwd?B=%s&F=%s' class=btnfunc>/ ת��</A>\n",
			board, file);
		nbuf += sprintf(buf + nbuf,
			"<DIV><A href='ccc?B=%s&F=%s' class=btnfunc>/ ת��</a>\n",
			board, file);

		if (num >= 0 && num < total) {
			brc_initial(currentuser.userid, board);
			brc_add_read(dirinfo);
			brc_update(currentuser.userid);
		}
		if (!strncmp(currentuser.userid, dirinfo->owner, IDLEN + 1)) {
			//|| has_BM_perm(&currentuser, bx)) {
			nbuf += sprintf(buf + nbuf,
				"<A onclick='return confirm(\"�����Ҫɾ��������?\")' href='del?B=%s&F=%s' class=btnfunc>/ ɾ��</a>\n",
				board, file);
			nbuf += sprintf(buf + nbuf,
				"<A href='edit?B=%s&F=%s' class=btnfunc>/ �޸�</a>\n",
				board, file);
		}
		dirinfo->title[sizeof(dirinfo->title) - 1] = 0;
		ptr = dirinfo->title;
		if (!strncmp(ptr, "Re: ", 4))
			ptr += 4;
		outgoing = (dirinfo->accessed & FH_INND) || strchr(dirinfo->owner, '.');

		fputs(buf, stdout);
		nbuf = 0;
		nbuf += sprintf(buf + nbuf,
			"<a href='pstmail?B=%s&F=%s&num=%d' class=btnfunc title=\"���Ÿ����� accesskey: m\" accesskey=\"m\">/ ���Ÿ�����</a>\n", board, file, num);
		nbuf += sprintf(buf + nbuf,
			"<a href='tfind?B=%s&th=%lu&T=%s' class=btnfunc>/ ͬ�����б�</a>\n", board, (long)dirinfo->thread, encode_url(ptr));
		nbuf += sprintf(buf + nbuf,
			"<a href='bbstcon?board=%s&start=%d&th=%lu' class=btnfunc>/ ͬ�����ɴ�չ��</a>\n", board, num, (long)dirinfo->thread);
		nbuf += sprintf(buf + nbuf,
			"<a href='%s%s&S=%d' class=btnfunc title=\"���������� accesskey: b\" accesskey=\"b\">/ ����������</a>\n",
			showByDefMode(), board, (num > 4) ? (num - 4) : 1);
		nbuf += sprintf(buf + nbuf, "</div></td></tr></table></td></tr>\n");
		nbuf += sprintf(buf + nbuf, "<tr><td width=\"60%%\">");

		if (!(dirinfo->accessed & FH_NOREPLY))
			nbuf += sprintf(buf + nbuf,
		"<a href='pst?B=%s&F=%s&num=%d%s' class=btnsubmittheme title=\"�ظ����� accesskey: r\" accesskey=\"r\">�ظ�����</a> </td>\n", board, file, num, outgoing ? "" : (inndboard ? "&la=1" : ""));

		nbuf += sprintf(buf + nbuf, "<td width=\"40%%\" align=right>���� ");
		char *encoded_title = url_encode(title_utf8);
		nbuf += sprintf(buf + nbuf, "<a href=\"#\" onclick=\"javascript:share('sina','%s','%s','%s');\"><img src=\"/images/share-sina.png\"/></a> ",encoded_title,board,file);
		nbuf += sprintf(buf + nbuf, "<a href=\"#\" onclick=\"javascript:share('renren','%s','%s','%s');\"><img src=\"/images/share-rr.png\"/></a> ",encoded_title,board,file);
		nbuf += sprintf(buf + nbuf, "<a href=\"#\" onclick=\"javascript:share('tencent','%s','%s','%s');\"><img src=\"/images/share-tencent.png\"/></a> | ",encoded_title,board,file);

		free(encoded_title);
		if (sametitle) {
			prenum = num - 1;
			nextnum = num + 1;
			while (prenum >= 0 && num - prenum < 100) {
				x = (struct fileheader *) (mf.ptr + prenum * sizeof (struct fileheader));
				if (x->thread == thread)
					break;
				prenum--;
			}
			if (prenum >= 0 && num - prenum < 100)
				nbuf += sprintf(buf + nbuf,
					"<a href='con?B=%s&F=%s&N=%d&st=1&T=%lu'>ͬ������ƪ </a>",
					board, fh2fname(x), prenum + 1, feditmark(*x));
			nbuf += sprintf(buf + nbuf,
					"<a href='%s%s&S=%d'>�������� </a>",
					showByDefMode(), board, (num > 4) ? (num - 4) : 1);
			while (nextnum < total && nextnum - num < 100) {
				x = (struct fileheader *) (mf.ptr + nextnum * sizeof (struct fileheader));
				if (x->thread == thread)
					break;
				nextnum++;
			}
			if (nextnum < total && nextnum - num < 100)
				nbuf += sprintf(buf + nbuf,
					"<a href='con?B=%s&F=%s&N=%d&st=1&T=%lu'>ͬ������ƪ</a>",
					board, fh2fname(x), nextnum + 1, feditmark(*x));
		} else {
			if (num > 0) {
				x = (struct fileheader *) (mf.ptr + (num - 1) * sizeof (struct fileheader));
				nbuf += sprintf(buf + nbuf,
					"<a href='con?B=%s&F=%s&N=%d&T=%lu' title=\"��ƪ accesskey: f\" accesskey=\"f\">��ƪ </a>",
					board, fh2fname(x), num, feditmark(*x));
			}
			nbuf += sprintf(buf + nbuf,
				"<a href='%s%s&S=%d' title=\"�������� accesskey: c\" accesskey=\"c\">�������� </a>",
				showByDefMode(), board, (num > 4) ? (num - 4) : 1);
			if (num < total - 1) {
				x = (struct fileheader *) (mf.ptr + (num + 1) * sizeof (struct fileheader));
				nbuf += sprintf(buf + nbuf,
					"<a href='con?B=%s&F=%s&N=%d&T=%lu' title=\"��ƪ accesskey: n\" accesskey=\"n\">��ƪ</a>",
					board, fh2fname(x), num + 2, feditmark(*x));
			}
		}
		nbuf += sprintf(buf+nbuf, "</td></tr></table></td></tr>\n");
	}
	MMAP_CATCH {
		mmapfile(NULL, &mf);
		MMAP_RETURN(-1);
	}
	MMAP_END mmapfile(NULL, &mf);

	fputs(buf, stdout);
	if (usexml)
		showconxml(filename, usexml);
	else
		fshowcon(stdout, filename, 0);

	printf("<tr><td></td><td height=\"20\" valign=\"middle\">");
	snprintf(fileback, sizeof(fileback), "://%s/%s/con?B=%s&F=%s", MY_BBS_DOMAIN, SMAGIC, board, file);
	printf("��������&nbsp;&nbsp;<a href='%s'>%s</a>", fileback, fileback);
	printf("</td></tr>");
	printf("</table></td></tr></table></td></tr></table>\n");
#ifdef ENABLE_MYSQL
	if (loginok && now_t - atoi(file + 2) <= 3 * 86400) {
		printf("<br /><script>eva('%s','%s');</script>", board, file);
	}
#endif

	processMath();
	printf("</body></html>\n");

	return 0;
}

