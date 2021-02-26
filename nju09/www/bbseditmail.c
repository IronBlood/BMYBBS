#include "bbslib.h"
#include "bmy/article.h"

static int update_form_mail(char *file, char *title, int box_type);

int
bbseditmail_main()
{
	FILE *fp;
	int type = 0, num;
	char buf[512], path[512], file[512], title[80];
	int base64, isa = 0;
	size_t len;
	char *fn = NULL;
	struct fileheader *x = NULL;
	struct mmapfile mf = { .ptr = NULL };
	html_header(1);
	check_msg();
	if (!loginok || isguest)
		http_fatal("�Ҵҹ��Ͳ����޸��ż������ȵ�¼");
	changemode(EDIT);
	ytht_strsncpy(title, getparm("title"), 60);
	ytht_strsncpy(file, getparm("F"), 30);
	type = atoi(getparm("type"));
	if (!file[0])
		ytht_strsncpy(file, getparm("file"), 30);

	int box_type = 0;
	ytht_strsncpy(buf, getparm("box_type"), 512);
	if(buf[0] != 0) {
		box_type = atoi(buf);
	}
	char type_string[20];
	snprintf(type_string, sizeof(type_string), "box_type=%d", box_type);

	if(box_type == 1) {
		setsentmailfile(path, currentuser.userid, ".DIR");
	} else {
		setmailfile_s(path, sizeof(path), currentuser.userid, ".DIR");
	}
	MMAP_TRY {
		if (mmapfile(path, &mf) == -1) {
			MMAP_UNTRY;
			http_fatal("�����������");
		}
		num = -1;
		x = findbarticle(&mf, file, &num, 1);
	}
	MMAP_CATCH {
		x = NULL;
	}
	MMAP_END mmapfile(NULL, &mf);
	if (strstr(file, "..") || strstr(file, "/"))
		http_fatal("����Ĳ���");
	if (x == 0)
		http_fatal("����Ĳ���");
	if (type != 0)
		return update_form_mail( file, title, box_type);

	printf("<body leftmargin=0 topmargin=0>\n");
	printf("<table width=\"100%%\" border=0 cellpadding=0 cellspacing=0>\n");
	printf("%s", "<tr>\n<td height=30 colspan=2>\n"
		"<table width=\"100%%\"  border=0 cellspacing=0 cellpadding=0>\n"
		"<tr><td width=40><img src=\"/images/spacer.gif\" width=40 height=10 alt=\"\"></td>\n"
		"<td><table width=\"100%%\" border=0 align=right cellpadding=0 cellspacing=0>\n"
		"<tr><td>\n");
	printf("%s", "<tr><td height=70 colspan=2>\n"
		"<table width=\"100%%\" height=\"100%%\" border=0 cellpadding=0 cellspacing=0 bgcolor=\"#efefef\">\n"
		"<tr><td width=40>&nbsp; </td>\n"
		"<td height=70><table width=\"95%%\" height=\"100%%\"  border=0 cellpadding=0 cellspacing=0>\n"
		"<tr>\n");
	printf("%s", "<tr><td width=40 class=\"level1\"></td>\n"
		"<td class=\"level1\"><br>\n"
		"<TABLE width=\"95%%\" cellpadding=5 cellspacing=0>\n"
		"<TBODY><TR><TD class=tdtitletheme>&nbsp;</TD>\n"
		"</TR>\n");
	printf("<TR><TD class=bordertheme>\n"
		"<form name=form1 method=post action=bbseditmail?%s>\n", type_string);
	printf("<table width=\"100%%\"  border=0 cellspacing=0 cellpadding=0>\n"
		"<tr>\n<td><table border=0 cellpadding=0 cellspacing=0>\n"
		"<tr><td> ʹ�ñ��⣺</td>\n"
		"<td><input name=title type=text class=inputtitle maxlength=45 size=50 value='%s' ></td>\n", (void1(nohtml(x->title))));
	printf("</tr></table></td></tr>");
	printf("%s", "<tr><td><table border=0 cellpadding=0 cellspacing=0><tr>\n");
	printf("<td> ���ߣ�%s &nbsp</td>\n<td>", fh2owner(x));

	printuploadattach();
	printf("</td></tr></table></td></tr>\n");
	printf("<tr><td><a href=home/boards/BBSHelp/html/itex/itexintro.html target=_blank>ʹ��Tex������ѧ��ʽ</a><input type=checkbox name=usemath%s>\n", x->accessed & FH_MATH ? " checked" : "");
	if(box_type == 1) {
		setsentmailfile(path, currentuser.userid,  file);
	} else {
		setmailfile_s(path, sizeof(path), currentuser.userid,  file);
	}
	fp = fopen(path, "r");
	if (fp == 0)
		http_fatal("�ļ���ʧ");
	snprintf(path, sizeof (path), PATHUSERATTACH "/%s", currentuser.userid);
	clearpath(path);
	keepoldheader(fp, SKIPHEADER);
	printf("<tr><td><textarea  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.form1.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=20 cols=76 wrap=virtual class=f2>\n");
	while (1) {
		if (fgets(buf, 500, fp) == 0)
			break;
		if (isa && (!strcmp(buf, "\r\n") || !strcmp(buf, "\n")))	//����֮����һ������
			continue;
		base64 = isa = 0;
		if (!strncmp(buf, "begin 644", 10)) {
			isa = 1;
			base64 = 1;
			len = 0;
			fn = buf + 10;
		} else if (checkbinaryattach(buf, fp, &len)) {
			isa = 1;
			base64 = 0;
			fn = buf + 18;
		}
		if (isa) {
			if (!getattach(fp, fn, path, base64, len, 0)) {
				printf("#attach %s\n", fn);
			}
		} else
			printf("%s", nohtml(void1(buf)));
	}
	fclose(fp);
	printf("</textarea></td></tr>\n");

	printf("<input type=hidden name=type value=1>\n");
	printf("<input type=hidden name=file value=%s>\n", file);
	printf("%s", "<tr><td><input name=Submit2 type=submit class=resetlong value=\"����\" "
			"onclick=\"this.value='�����ύ�У����Ժ�...';this.disabled=true;form1.submit();\">\n"
			"<input name=Submit3 type=reset class=sumbitlong value=\"����\" onclick='return confirm(\"ȷ��Ҫȫ�������?\")'></td>\n"
			"</tr>\n");
	printf("%s", "</table></TD></TR></TBODY></TABLE></form></td></tr>\n"
		"<tr>\n<td height=40 bgcolor=\"#FFFFFF\">��</td>\n"
		"<td height=40 bgcolor=\"#FFFFFF\">��</td>\n"
		"</tr></table></td></tr></table>\n");
/*
	printf("<tr><td class=post align=center>\n");
	printf("<input type=submit value=����> \n");
	printf("<input type=reset value=����></form>\n");
	printf("</table>");
*/
	http_quit();
	return 0;
}

static int update_form_mail(char *file, char *title, int box_type) {
	FILE *fp;
	char *buf = getparm("text"), path[80];
	int num = 0, filetime;
	int usemath, useattach, nore;
	char dir[STRLEN];
	char filename[STRLEN];
	struct fileheader x;
	struct mmapfile mf = { .ptr = NULL };
	size_t i;
	long l;

	char type_string[20];
	snprintf(type_string, sizeof(type_string), "box_type=%d", box_type);

	filetime = atoi(file + 2);
	usemath = strlen(getparm("usemath"));
	nore = strlen(getparm("nore"));
	if (usemath)
		usemath = bmy_article_include_math(buf);

	for (i = 0; i < strlen(title); i++)
		if (title[i] <= 27 && title[i] >= -1)
			title[i] = ' ';
	l = strlen(title) - 1;
	while (l >= 0 && isspace(title[l])) {
		title[l] = 0;
		l--;
	}
	if (title[0] == 0)
		http_fatal("���ⲻ��Ϊ��");
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	useattach = (insertattachments(filename, buf, currentuser.userid));
	if(box_type == 1) {
		setsentmailfile(path, currentuser.userid, file);
	} else {
		setmailfile_s(path, sizeof(path), currentuser.userid, file);
	}
	fp = fopen(path, "r+");
	if (fp == 0)
		http_fatal("�޷�����");
	keepoldheader(fp, SKIPHEADER);
	mmapfile(filename, &mf);
	fwrite(mf.ptr, mf.size, 1, fp);
	mmapfile(NULL, &mf);
	unlink(filename);
	fclose(fp);
	add_edit_mark(path, currentuser.userid, now_t, fromhost);
	if(box_type == 1) {
		setsentmailfile(dir, currentuser.userid, ".DIR");
	} else {
		setmailfile_s(dir, sizeof(dir), currentuser.userid, ".DIR");
	}
	fp = fopen(dir, "r");
	if (fp == 0)
		http_fatal("����Ĳ���");
	while (1) {
		if (fread(&x, sizeof (struct fileheader), 1, fp) <= 0)
			break;
		if (x.filetime == filetime) {
			x.edittime = now_t;
			x.sizebyte = ytht_num2byte(eff_size(path));
			ytht_strsncpy(x.title, title, sizeof(x.title));
			if (nore)
				x.accessed |= FH_NOREPLY;
			else
				x.accessed &= ~FH_NOREPLY;
			if (usemath)
				x.accessed |= FH_MATH;
			else
				x.accessed &= ~FH_MATH;
			if (useattach)
				x.accessed |= FH_ATTACHED;
			else
				x.accessed &= ~FH_ATTACHED;
			put_record(&x, sizeof (struct fileheader), num, dir);
			break;
		}
		num++;
	}
	fclose(fp);
	printf("�޸��ż��ɹ�.<br><a href=bbsmail?%s>�����ż��б�</a>", type_string);
	return 0;
}

