#include "bbslib.h"
#include "ythtbbs/identify.h"

#ifdef POP_CHECK
// ��½�ʼ��������õ�ͷ�ļ� added by interma@BMY 2005.5.12
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// �ʼ����������û���������ĳ��ȣ� added by interma@BMY 2005.5.12
#define USER_LEN 20
#define PASS_LEN 20

#endif

#ifdef POP_CHECK
// ��username�û�ͨ����֤�� added by interma@BMY 2005.5.12
void register_success(int usernum, char *userid, char *realname, char *dept,
char *addr, char *phone, char *assoc, char *email)
{
	struct userec uinfo;
	FILE *fout, *fn;
	char buf[STRLEN];
	time_t n;
	char genbuf[512];

	//int id = getuser(userid);
	struct userec *u = getuser(userid);

	sethomefile(genbuf, userid, "mailcheck");
	//http_fatal(genbuf);
	if ((fn = fopen(genbuf, "w")) == NULL) {
		fclose(fn);
		return;
	}

	fprintf(fn, "usernum: %d\n", usernum);
	fclose(fn);

	memcpy(&uinfo, u, sizeof (uinfo));

	ytht_strsncpy(uinfo.userid, userid, sizeof(uinfo.userid));
	ytht_strsncpy(uinfo.realname, realname, sizeof(uinfo.realname));
	ytht_strsncpy(uinfo.address, addr, sizeof(uinfo.address));
	sprintf(genbuf, "%s$%s@%s", dept, phone, userid);
	ytht_strsncpy(uinfo.realmail, genbuf, sizeof(uinfo.realmail));

	ytht_strsncpy(uinfo.email, email, sizeof(uinfo.email));

	uinfo.userlevel |= PERM_DEFAULT;	// by ylsdd
	substitute_record(PASSFILE, &uinfo, sizeof (struct userec), usernum);

	sethomefile(buf, uinfo.userid, "sucessreg");
	if ((fout = fopen(buf, "w")) != NULL) {
		fprintf(fout, "\n");
		fclose(fout);
	}

	sethomefile(buf, uinfo.userid, "register");

	if ((fout = fopen(buf, "w")) != NULL) {
		fprintf(fout, "%s: %d\n", "usernum", usernum);
		fprintf(fout, "%s: %s\n", "userid", userid);
		fprintf(fout, "%s: %s\n", "realname", realname);
		fprintf(fout, "%s: %s\n", "dept", dept);
		fprintf(fout, "%s: %s\n", "addr", addr);
		fprintf(fout, "%s: %s\n", "phone", phone);
		fprintf(fout, "%s: %s\n", "assoc", assoc);

		n = time(NULL);
		fprintf(fout, "Date: %s\n", ctime(&n));
		fprintf(fout, "Approved: %s\n", userid);
		fclose(fout);
	}

	mail_file("etc/s_fill", uinfo.userid, "������ͨ�������֤", "SYSOP");

	mail_file("etc/s_fill2", uinfo.userid, "��ӭ����" MY_BBS_NAME "���ͥ", "SYSOP");
	sethomefile(buf, uinfo.userid, "mailcheck");
	unlink(buf);
	sprintf(genbuf, "�� %s ͨ�����ȷ��.", uinfo.userid);
	securityreport(genbuf, genbuf);
	return ;
}

#endif
// -------------------------------------------------------------------------------


#if 0
int
badymd(int y, int m, int d)
{
	int max[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
		max[2] = 29;
	if (y < 10 || y > 100 || m < 1 || m > 12)
		return 1;
	if (d < 0 || d > max[m])
		return 1;
	return 0;
}
#endif

int
bbsdoreg_main()
{
	FILE *fp;
	struct userec x;
	char buf[256], filename[80], pass1[80], pass2[80], dept[80], phone[80],
	    assoc[80], salt[3], words[1024], *ub = FIRST_PAGE;
	int lockfd;
	struct active_data act_data;
	html_header(1);
	printf("<body>");
	bzero(&x, sizeof (x));
//      xz=atoi(getparm("xz"));

#ifdef POP_CHECK
	char user[USER_LEN + 1];
	char popserver[4];
	int popserver_index;
	ytht_strsncpy(popserver, getparm("popserver"), 4);
	ytht_strsncpy(user, getparm("user"), USER_LEN);
#endif

	ytht_strsncpy(x.userid, getparm("userid"), 13);
	ytht_strsncpy(pass1, getparm("pass1"), 13);
	ytht_strsncpy(pass2, getparm("pass2"), 13);
	ytht_strsncpy(x.username, getparm("username"), 32);
	ytht_strsncpy(x.realname, getparm("realname"), 32);
	ytht_strsncpy(dept, getparm("dept"), 60);
	ytht_strsncpy(x.address, getparm("address"), 60);

#ifndef POP_CHECK
	strsncpy(x.email, getparm("email"), 60);
#else
	const char *popname;
	popserver_index = atoi(popserver);
	if(popserver_index < 1 || popserver_index > 3) popserver_index = 1;

	popname = MAIL_DOMAINS[popserver_index];

	char email[60];
	snprintf(email, 60, "%s@%s", user, popname);  // ע�ⲻҪ��emailŪ�����
	ytht_str_to_lowercase(email);
	ytht_strsncpy(x.email, email, 60);
#endif

	ytht_strsncpy(phone, getparm("phone"), 60);
	ytht_strsncpy(assoc, getparm("assoc"), 60);
	ytht_strsncpy(words, getparm("words"), 1000);

    if (id_with_num(x.userid))
		http_fatal("�ʺ�ֻ����Ӣ����ĸ���");
	if (strlen(x.userid) < 2)
		http_fatal("�ʺų���̫��(2-12�ַ�)");
	if (strcasecmp(x.userid, "new") == 0)
		http_fatal("��ֹʹ�� new ��Ϊ�˺���");
	if (strlen(pass1) < 4)
		http_fatal("����̫��(����4�ַ�)");
	if (strcmp(pass1, pass2))
		http_fatal("������������벻һ��, ��ȷ������");
	if (strlen(x.username) < 2)
		http_fatal("�������ǳ�(�ǳƳ�������2���ַ�)");
	if (strlen(x.realname) < 4)
		http_fatal("��������ʵ����(��������, ����2����)");
//      if(strlen(dept)<6) http_fatal("������λ�����Ƴ�������Ҫ6���ַ�(��3������)");
	if (strlen(x.address) < 6)
		http_fatal("ͨѶ��ַ��������Ҫ6���ַ�(��3������)");
	if (badstr(x.passwd) || badstr(x.username) || badstr(x.realname))
		http_fatal("����ע�ᵥ�к��зǷ��ַ�");
	if (badstr(x.address) || badstr(x.email))
		http_fatal("����ע�ᵥ�к��зǷ��ַ�");
	if (is_bad_id(x.userid))
		http_fatal("�����ʺŻ��ֹע���id, ������ѡ��");
	if (getuser(x.userid))
		http_fatal("���ʺ��Ѿ�����ʹ��,������ѡ��");
//      sprintf(salt, "%c%c", 65+rand()*26, 65+rand()*26);
//add by lepton


#ifdef POP_CHECK
	if (strlen(user) == 0)
		http_fatal("�����û���û��");
	if (check_mail_to_address(email) == MAIL_SENDER_WRONG_EMAIL)
		http_fatal("�������������Ϸ�������ϵվ������ https://github.com/bmybbs/bmybbs/issues/ �������⡣");
#endif

	getsalt(salt);
	ytht_strsncpy(x.passwd, ytht_crypt_crypt1(pass1, salt), 14);
	//ipv6 by leoncom ���ܸ�ֵ̫�࣬��Ӱ���������� fixed by IronBlood 2020.09.11
	strncpy(x.lasthost, fromhost,BMY_IPV6_LEN);
	x.userlevel = PERM_BASIC;
	x.firstlogin = now_t;
	x.lastlogin = now_t - 3600;  //ipv6 by leoncom ע����ֶ���¼
	x.userdefine = -1;
	x.flags[0] = CURSOR_FLAG | PAGER_FLAG;
//      if(xz==1) currentuser.userdefine ^= DEF_COLOREDSEX;
//      if(xz==2) currentuser.userdefine ^= DEF_S_HOROSCOPE;
	adduser(&x);

#ifndef POP_CHECK
	lockfd = openlockfile(".lock_new_register", O_RDONLY, LOCK_EX);
	fp = fopen("new_register", "a");
	if (fp) {
		fprintf(fp, "usernum: %d, %s\n", getusernum(x.userid) + 1,
			ytht_ctime(now_t));
		fprintf(fp, "userid: %s\n", x.userid);
		fprintf(fp, "realname: %s\n", x.realname);
		fprintf(fp, "dept: %s\n", dept);
		fprintf(fp, "addr: %s\n", x.address);
		fprintf(fp, "phone: %s\n", phone);
		fprintf(fp, "assoc: %s\n", assoc);
		fprintf(fp, "----\n");
		fclose(fp);
	}
	close(lockfd);
#endif

	sprintf(filename, "home/%c/%s", mytoupper(x.userid[0]), x.userid);
	mkdir(filename, 0755);

#ifndef POP_CHECK
	printf("<center><table><td><td><pre>\n");
	printf("�װ�����ʹ���ߣ����ã�\n\n");
	printf("��ӭ���� ��վ, �������ʺ��Ѿ��ɹ����Ǽ��ˡ�\n");
	printf("��Ŀǰӵ�б�վ������Ȩ��, �����Ķ����¡������ķ�������˽��\n");
	printf("�ż����������˵���Ϣ�����������ҵȵȡ�����ͨ����վ�����ȷ\n");
	printf("������֮���������ø����Ȩ�ޡ�Ŀǰ����ע�ᵥ�Ѿ����ύ\n");
	printf("�ȴ����ġ�һ�����24Сʱ���ھͻ��д𸴣������ĵȴ���ͬʱ��\n");
	printf("��������վ�����䡣\n");
	printf("��������κ����ʣ�����ȥsysop(վ���Ĺ�����)�淢��������\n\n</pre></table>");
	printf("<hr><br>���Ļ�����������:<br>\n");
	printf("<table border=1 width=400>");
	printf("<tr><td>�ʺ�λ��: <td>%d\n", getusernum(x.userid));
	printf("<tr><td>ʹ���ߴ���: <td>%s (%s)\n", x.userid, x.username);
	printf("<tr><td>��  ��: <td>%s<br>\n", x.realname);
	printf("<tr><td>��  ��: <td>%s<br>\n", x.username);
	printf("<tr><td>��վλ��: <td>%s<br>\n", x.lasthost);
	printf("<tr><td>�����ʼ�: <td>%s<br></table><br>\n", x.email);

	printf("<center><form><input type=button onclick='window.close()' value=�رձ�����></form></center>\n");
#else
	printf("<center><table><td><td><pre>\n");

	memset(&act_data, 0, sizeof(act_data));
	snprintf(act_data.name, NAMELEN, "%s", x.realname);
	act_data.name[NAMELEN-1] = '\0';
	strcpy(act_data.userid, x.userid);
	snprintf(act_data.dept, STRLEN, "%s", dept);
	act_data.dept[STRLEN-1] = '\0';
	snprintf(act_data.phone, VALUELEN, "%s", phone);
	act_data.phone[VALUELEN-1] = '\0';
	snprintf(act_data.email, VALUELEN, "%s", email);
	act_data.email[VALUELEN-1] = '\0';
	strcpy(act_data.ip, fromhost);
	strcpy(act_data.operator, x.userid);

	act_data.status=0;
	write_active(&act_data);

	int result;
	if (strcasecmp(user, "test") == 0) {
		result = -2; // ����
	} else if (query_record_num(email, MAIL_ACTIVE) >= MAX_USER_PER_RECORD) {
		result = -3;
	} else {
		// smtp
		result = send_active_mail(x.userid, email);
	}
	switch (result)
	{
	case -2:
		printf("<tr><td>%s<br></table><br>\n",
				"��ӭ�����뽻����������ٸBBS��<br>��������������������ע�ᣬĿǰ���������û���ݡ�"
				"Ŀǰ��û�з��ġ��ż�����Ϣ��Ȩ�ޡ�<br><br>"
				"���ڿ�ѧȡ��stu.xjtu.edu.cn�����<br>�������������дע�ᵥ��������������֤��������Ϊ��վ��ʽ�û���");
		break;
	case -3:
		printf("���������Ѿ���֤�� %d ��id���޷���������֤��!\n", MAX_USER_PER_RECORD);
		break;
	case -1:
	case 0:
		printf("<tr><td>%s<br></table><br>\n", "�ʼ�������������ʧ�ܣ�����ֻ��ʹ�ñ�bbs����������ܣ�ʮ�ֱ�Ǹ��");
		break;

	case 1:
		printf("��ӭ�����뽻����������ٸBBS��<br>"
			"Ŀǰ��û�з��ġ��ż�����Ϣ��Ȩ�ޡ�<br>"
			"��֤��Ϣ�ѷ������������� %s ����ʱ����ա�<br>"
			"���¼ϵͳ��������������дע�ᵥ��������������֤��������Ϊ��վ��ʽ�û���", email);
		break;
	}

	printf("<center><form><input type=button onclick='window.close()' value=�رձ�����></form></center>\n");
#endif

	// �������У�newcomer�����ܽ�������www��ia64���µ����⡣interma@BMY
	newcomer(&x, words);


//      sprintf(buf, "%s %-12s %d\n", ytht_ctime(now_t)+4, x.userid, getusernum(x.userid));
//      f_append("wwwreg.log", buf);
	sprintf(buf, "%s newaccount %d %s www", x.userid, getusernum(x.userid), fromhost);
	newtrace(buf);
	//wwwstylenum = 1;

	//don't login with reg by leoncom for ipv6
	//ub = wwwlogin(&x,0);
	//sprintf(buf, "%s enter %s www", x.userid, fromhost);
	//newtrace(buf);
	//printf("<script>opener.top.location.href=\"%s\";</script>", ub);
	return 0;
}

void
newcomer(struct userec *x, char *words)
{
	FILE *fp;
	char filename[80];
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	fp = fopen(filename, "w");
	fprintf(fp, "��Һ�, \n\n");
	fprintf(fp, "���� %s(%s), ���� %s\n", x->userid, x->username, fromhost);
	fprintf(fp, "��������˵ر���, ���Ҷ��ָ��.\n\n");
	fprintf(fp, "���ҽ���:\n\n");
	fprintf(fp, "%s", words);
	fclose(fp);
	post_article("newcomers", "WWW������·", filename, x->userid, x->username, fromhost, -1, 0, 0, x->userid, -1);
	unlink(filename);
}

void
adduser(struct userec *x)
{
	int i;
	FILE *fp;
	fp = fopen(".PASSWDS", "r+");
	flock(fileno(fp), LOCK_EX);
	for (i = 0; i < MAXUSERS; i++) {
		if (shm_ucache->userid[i][0] == 0) {
			if (i + 1 > shm_ucache->number)
				shm_ucache->number = i + 1;
			strncpy(shm_ucache->userid[i], x->userid, 13);
			insertuseridhash(uidhashshm->uhi, UCACHE_HASH_SIZE, x->userid, i + 1);
			save_user_data(x);
			break;
		}
	}
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
}
