//ylsdd Nov 05, 2002
#include <dirent.h>
#include "bbs.h"
#include "ythtbbs/ythtbbs.h"
#include "bmy/cookie.h"
#include "ythtbbs/session.h"

static char *FileName;		/* The filename, as selected by the user. */
static char *ContentStart;	/* Pointer to the file content. */
static int ContentLength;	/* Bytecount of the content. */
char userattachpath[256];
int attachtotalsize;

static char *
getsenv(char *s)
{
	char *t = getenv(s);
	if (t)
		return t;
	return "";
}

static char *
getreqstr()
{
	static char str[100] = { 0 }, *ptr;
	if (str[0])
		return str;
	ytht_strsncpy(str, getsenv("SCRIPT_URL"), sizeof(str));
	if ((ptr = strchr(str, '&')))
		*ptr = 0;
	return str;
}

int
getpathsize(char *path, int showlist)
{
	DIR *pdir;
	struct dirent *pdent;
	char fname[1024];
	int totalsize = 0, size;
	if (showlist)
		printf("�Ѿ����صĸ�����:<br>");
	pdir = opendir(path);
	if (!pdir)
		return -1;
	while ((pdent = readdir(pdir))) {
		if (!strcmp(pdent->d_name, "..") || !strcmp(pdent->d_name, "."))
			continue;
		if (strlen(pdent->d_name) + strlen(path) >= sizeof (fname)) {
			totalsize = -1;
			break;
		}
		sprintf(fname, "%s/%s", path, pdent->d_name);
		size = ytht_file_size_s(fname);
		printf("<ul>\n");
		if (showlist) {
			printf("<li> <b>%s</b> (<i>%d�ֽ�</i>) ", pdent->d_name, size);
			printf("<a href='%s&%s'>ɾ��</a></li>\n", getreqstr(), pdent->d_name);
		}
		printf("</ul>\n");
		if (size < 0) {
			totalsize = -1;
			break;
		}
		totalsize += size;
	}
	closedir(pdir);
	if (showlist) {
		printf("<br>��С�ܼ� %d �ֽ� (��� %d �ֽ�)<br>", totalsize, MAXATTACHSIZE);
	}
	return totalsize;
}

static void
html_header()
{
	printf("Content-type: text/html; charset=gb2312\n\n\n");
	printf("<!DOCTYPE html><HTML><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gbk\"/><title>upload-BMYBBS</title></head><body bgcolor=#f0f4f0>\n");
}

static void
http_fatal(char *str)
{
	printf("����! %s!<br>", str);
	printf("<a href=javascript:history.go(-1)>���ٷ���</a></body></html>");
	exit(0);
}

/**
 * Skip a line in the input stream.
 * @param Input        Pointer into the incoming stream.
 * @param InputLength  Bytes left in the incoming stream.
 */
static void SkipLine(char **Input, int *InputLength) {
	while ((**Input != '\0') && (**Input != '\r') && (**Input != '\n')) {
		*Input = *Input + 1;
		*InputLength = *InputLength - 1;
	}
	if (**Input == '\r') {
		*Input = *Input + 1;
		*InputLength = *InputLength - 1;
	}
	if (**Input == '\n') {
		*Input = *Input + 1;
		*InputLength = *InputLength - 1;
	}
}

/**
 * @brief TODO
 * @param Input        Pointer into the incoming stream.
 * @param InputLength  TODO
 * @param len          TODO
 */
static void GoAhead(char **Input, int *InputLength, int len) {
	*Input += len;
	*InputLength -= len;
}

/**
 * Accept a single segment from the incoming mime stream. Each field in the
 * form will generate a mime segment. Return a pointer to the beginning of
 * the Boundary, or NULL if the stream is exhausted.
 * @param Input        Pointer into the incoming stream.
 * @param InputLength  Bytes left in the incoming stream.
 * @param Boundary     Character string that delimits segments.
 */
static void AcceptSegment(char **Input, int *InputLength, char *Boundary) {
	char *FieldName;	/* Name of the variable from the form. */
	char *ContentEnd;
	char *contentstr, *ptr;
	/* The input stream should begin with a Boundary line. Error-exit if not found. */
	if (strncmp(*Input, Boundary, strlen(Boundary)) != 0)
		http_fatal("�ļ����ʹ��� 10");
	/* Skip the Boundary line. */
	GoAhead(Input, InputLength, strlen(Boundary));
	SkipLine(Input, InputLength);
	/* Return NULL if the stream is exhausted (no more segments). */
	if ((**Input == '\0') || (strncmp(*Input, "--", 2) == 0))
		http_fatal("�ļ����ʹ��� 11");
	// The first line of a segment must be a "Content-Disposition" line. It
	// contains the fieldname, and optionally the original filename. Error-exit
	// if the line is not recognised.
	contentstr = "content-disposition: form-data; name=\"";
	if (strncasecmp(*Input, contentstr, strlen(contentstr)))
		http_fatal("�ļ����ʹ��� 12");
	GoAhead(Input, InputLength, strlen(contentstr));
	FieldName = *Input;
	ptr = strchr(*Input, '\"');
	if (!ptr)
		http_fatal("�ļ����ʹ��� 13");
	*ptr = 0;
	ptr++;
	while (*ptr && !isalpha(*ptr))
		ptr++;
	GoAhead(Input, InputLength, ptr - *Input);
	if (strncasecmp(*Input, "filename=\"", strlen("filename=\"")))
		http_fatal("�ļ����ʹ��� 14");
	GoAhead(Input, InputLength, strlen("filename=\""));
	FileName = *Input;
	ptr = strchr(*Input, '\"');
	if (!ptr)
		http_fatal("�ļ����ʹ��� 15");
	*ptr = 0;
	ptr++;
	GoAhead(Input, InputLength, ptr - *Input);
	// Skip the Disposition line and one or more mime lines, until an empty
	// line is found.
	SkipLine(Input, InputLength);
	while ((**Input != '\r') && (**Input != '\n'))
		SkipLine(Input, InputLength);
	SkipLine(Input, InputLength);
	// The following data in the stream is binary. The Boundary string is the
	// end of the data. There may be a CRLF just before the Boundary, which
	// must be stripped.
	ContentStart = *Input;
	ContentLength = 0;
	while (*InputLength > 0 && memcmp(*Input, Boundary, strlen(Boundary))) {
		GoAhead(Input, InputLength, 1);
		ContentLength++;
	}
	ContentEnd = *Input - 1;
	if ((ContentLength > 0) && (*ContentEnd == '\n')) {
		ContentEnd--;
		ContentLength--;
	}
	if ((ContentLength > 0) && (*ContentEnd == '\r')) {
		ContentEnd--;
		ContentLength--;
	}
}

static int
myatoi(char *a)
{
	int i = 0;
	while ((unsigned char)*a)
		i = i * 26 + (*(a++) - 'A');
	return i;
}

int
save_attach()
{
	char *ptr, *p0, filename[1024];
	char *pSuffix;//add by wsf
	int suffixLen;//add by wsf
	FILE *fp;


	p0 = FileName;
	ptr = strrchr(p0, '/');
	if (ptr) {
		*ptr = 0;
		ptr++;
	} else
		ptr = p0;
	p0 = ptr;
	ptr = strrchr(p0, '\\');
	if (ptr) {
		*ptr = 0;
		ptr++;
	} else
		ptr = p0;
	p0 = ptr;
	//�޸Ŀ�ʼ add by wsf
	if (strlen(p0) > 40){//�ļ��������ض�ʱ������׺��
		pSuffix = strrchr(p0,'.');
		suffixLen = strlen(pSuffix);

		int pAscii=40-suffixLen;
		while((unsigned int)p0[pAscii]>0xA0)
			--pAscii;
		pAscii = (pAscii+1) & 1;
		strcpy(&p0[40-suffixLen-pAscii],pSuffix);
		printf("<script language=\"JavaScript\">alert('�ļ�������40���ַ����ȣ��Ѿ������˽�ȡ');</script>\n");
	}

	if (checkfilename(p0)){
		printf("<script language=\"JavaScript\">\n"
				" alert(\"�ļ����в��ܰ����ո��������ķǷ��ַ�\\r\\n\\/~`!@#$%%^&*()|{}[];:\\\"'<>,?\");\n"
				"</script>\n");
		http_fatal("��Ч���ļ���");
	}
	//�޸Ľ��� add by wsf
	sprintf(filename, "%s/%s", userattachpath, p0);
	fp = fopen(filename, "w");
	fwrite(ContentStart, 1, ContentLength, fp);
	fclose(fp);
	printf("�ļ� %s (%d�ֽ�) ���سɹ�<br>", p0, ContentLength);
	return 0;
}

void
do_del()
{
	char str[1024], *p0, *ptr, str_gbk[1024];
	char filename[1024];
	ytht_strsncpy(str, getsenv("PATH_INFO"), sizeof(str));
	if (!(p0 = strchr(str, '&')))
		return;
	p0++;
	strcpy(str, p0);
	if(is_utf(str, strlen(str))){
		printf("<span style=\"color: red\">utf8</span><br />");
		u2g(str,strlen(str),str_gbk,sizeof(str_gbk));
		p0=str_gbk;
	}
	else
		p0=str;

	ptr = strsep(&p0, "&");

	if (checkfilename(ptr))
		http_fatal("��Ч���ļ���");
	sprintf(filename, "%s/%s", userattachpath, ptr);

	if( access(filename, F_OK) != -1){
		printf("<span style=\"color: red\">��ɾ��: %s</span><br />", ptr);
		unlink(filename);
	}
	else{
		printf("<span style=\"color: red\">����: %s ������</span><br />", ptr);
	}

}

void
printuploadform()
{
	char *req = getreqstr();
	printf("<hr>"
			"<form name=frmUpload action='%s' enctype='multipart/form-data' method=post>"
			"���ظ���: <input type=file name=file>"
			"<input type=submit value=���� "
			"onclick=\"this.value='���������У����Ժ�...';this.disabled=true;frmUpload.submit();\">"
			"</form> "
			"���������Ϊ���¸��ӵ�СͼƬС����ɶ��, ��Ҫ��̫��Ķ���Ŷ, �ļ�������Ҳ��Ҫ�������ʺ�ʲô��, �����ճ��ʧ��Ŷ.<br>"
			"<b>���������������ⶨλ����</b>��ֻ��Ҫ�����±༭����Ԥ��λ���϶�ͷд�ϡ�#attach 1.jpg���Ϳ�����(�����˽� 1.jpg ���������ص��ļ��� :) )<br>"
			"��Ҫע�⣬�����ͼƬ�Ļ���<b>����ͼƬ�ļ�����СΪ %d byte</b>."
			"<center><input type=button value='ˢ��' onclick=\"location='%s';\">&nbsp; &nbsp;"
			"<input type=button value='���' onclick='window.close();'></center>",
			req, MAXPICSIZE, req);
}

int
main(int argc, char *argv[], char *environment[])
{
	char *ptr, *buf;
	char Boundary[1024] = "--";
	int len, i;
	const struct user_info *ptr_info;
	char cookie_buf[128];
	struct bmy_cookie cookie;

	html_header();
	seteuid(BBSUID);

	if (geteuid() != BBSUID)
		http_fatal("�ڲ����� 0");
	ythtbbs_cache_utmp_resolve();

	ytht_strsncpy(cookie_buf, getsenv("HTTP_COOKIE"), sizeof(cookie_buf));
	memset(&cookie, 0, sizeof(struct bmy_cookie));
	bmy_cookie_parse(cookie_buf, &cookie);

	i = ythtbbs_session_get_utmp_idx(cookie.sessid, cookie.userid);
	if (i < 0 || i > USHM_SIZE)
		http_fatal("���ȵ�¼ 2");
	ptr_info = ythtbbs_cache_utmp_get_by_idx(i);
	if (!ptr_info->active)
		http_fatal("���ȵ�¼ 31");
	if (!(ptr_info->userlevel & PERM_POST))
		http_fatal("ȱ�� POST Ȩ��");
	snprintf(userattachpath, sizeof (userattachpath), PATHUSERATTACH "/%s", ptr_info->userid);
	mkdir(userattachpath, 0760);
	//clearpath(userattachpath);
	/* Test if the program was started by a METHOD=POST form. */
	if (strcasecmp(getsenv("REQUEST_METHOD"), "post")) {
		do_del();
		attachtotalsize = getpathsize(userattachpath, 1);
		if (attachtotalsize < MAXATTACHSIZE)
			printuploadform();
		printf("</body></html>");
		return 0;
	}
	attachtotalsize = getpathsize(userattachpath, 0);
	if (attachtotalsize < 0)
		http_fatal("�޷����Ŀ¼��С");
	/* Test if the program was started with ENCTYPE="multipart/form-data". */
	ptr = getsenv("CONTENT_TYPE");
	if (strncasecmp(ptr, "multipart/form-data; boundary=", 30))
		http_fatal("�ļ����ʹ��� 2");
	// Determine the Boundary, the string that separates the segments in the
	// stream. The boundary is available from the CONTENT_TYPE environment
	// variable.
	ptr = strchr(ptr, '=');
	if (!ptr)
		http_fatal("�ļ����ʹ��� 3");
	ptr++;
	ytht_strsncpy(Boundary + 2, ptr, sizeof(Boundary) - 2);
	// Get the total number of bytes in the input stream from the
	// CONTENT_LENGTH environment variable.
	len = atoi(getsenv("CONTENT_LENGTH"));
	if (len <= 0 || len > 5000000)
		http_fatal("�ļ����ʹ��� 4");
	buf = malloc(len + 1);
	if (!buf)
		http_fatal("�ļ����ʹ��� 5");
	len = fread(buf, 1, len, stdin);
	buf[len] = 0;
	ptr = buf;
	AcceptSegment(&ptr, &len, Boundary);

	// ͼƬ��С����
	ptr = FileName+strlen(FileName);
	if (!strcasecmp(ptr - 4, ".gif") || !strcasecmp(ptr - 4, ".jpg") ||
			!strcasecmp(ptr - 4, ".bmp") || !strcasecmp(ptr - 4, ".png")) {
		if (ContentLength > MAXPICSIZE) {
			free(buf);
			http_fatal("ͼƬ����̫��, �����޶�");
		}
	}

	if (ContentLength + attachtotalsize > MAXATTACHSIZE) {
		free(buf);
		http_fatal("����̫��, �����޶�");
	}
	save_attach();
	/* Cleanup. */
	free(buf);
	attachtotalsize = getpathsize(userattachpath, 1);
	if (attachtotalsize < MAXATTACHSIZE)
		printuploadform();
	return (0);
}

