#include "bbslib.h"

static int bm_printboard(struct boardmanager *bm, void *farg);

// const char *, int *, char *
static int fuzzy_matching_userid_v(const struct ythtbbs_cache_User *user, int curr_idx, va_list ap) {
	const char *userid;
	int *j;
	char *buf;

	userid = va_arg(ap, const char *);
	j = va_arg(ap, int *);
	buf = va_arg(ap, char *);

	if (strcasestr(user->userid, userid) == user->userid) {
		// user->userid ����Ѱ�ҵ� userid ��ͷ�Ĳ���
		*j = *j + 1;
		if (*j % 6 == 1)
			printf("<tr>");

		printf("<td>");
		printf("<a href=bbsqry?userid=%s>%s</a>", user->userid, user->userid);
		printf("</td>");
		sprintf(buf, "bbsqry?userid=%s", user->userid);
		if (*j % 6 == 0)
			printf("</tr>");
		if (*j >= 12 * 6)
			return QUIT;
	}

	return 0;
}

int show_special_web(char *id2) {
	FILE *fp;
	char id1[80], name[80], buf[256];
	fp=fopen("etc/special", "r");
	if(fp==0)
		return 0;
	while(1) {
		if(fgets(buf, 256, fp)==0) break;
		if(sscanf(buf, "%s %s", id1, name)<2) continue;
		if(!strcasecmp(id1, id2)) hprintf(" \033[1;33m��\033[36m%s\033[33m��\033[m",name);
	}
	fclose(fp);
	return 0;
}

void show_special_api(char *id2, char *output){
	FILE *fp;
	char id1[80], name[80], buf[256];
	fp=fopen("etc/special", "r");
	if(fp!=0){
		while(1){
			if(fgets(buf, 256, fp)==0) break;
			if(sscanf(buf, "%s %s", id1, name)<2) continue;
			if(!strcasecmp(id1, id2)) sstrcat(output, "\"Title\":\"%s\",", name);
		}
	}
	fclose(fp);
}

int
bbsqry_main()
{
	FILE *fp;
	char userid[14], filename[80], buf[512];
	struct userec *x;
	const struct user_info *u;
	int i, tmp2, num;
	html_header(1);
	check_msg();
	changemode(QUERY);
	ytht_strsncpy(userid, getparm("U"), 13);
	if (!userid[0])
		ytht_strsncpy(userid, getparm("userid"), 13);
	printf("<body><center>");
	printf("<div class=rhead>%s -- ��ѯ����</div><hr>\n", BBSNAME);
	if (userid[0] == 0) {
		printf("<form action=bbsqry>\n");
		printf("�������û���: <input name=userid maxlength=12 size=12>\n");
		printf("<input type=submit value=��ѯ�û�>\n");
		printf("</form><hr>\n");
		http_quit();
	}
	x = getuser(userid);
	if (x == 0) {
		int j = 0;
		printf("û������û������ѵ�����Щ:<p>");
		printf("<table width=600>");
		ythtbbs_cache_UserTable_foreach_v(fuzzy_matching_userid_v, userid, &j, buf);
		printf("</table>");
		if (j == 0)
			printf("�����ܣ��϶������ô��ˣ�����û���˰�");
		if (j == 1)
			redirect(buf);
		printf("<p><a href=javascript:history.go(-1)>���ٷ���</a>");
		http_quit();
	}
	printf("</center><pre style='font-size:14px'>\n");
	sprintf(buf,
		"%s (\033[33m%s\033[37m) ����վ \033[1;32m%d\033[m �Σ��������� \033[1;32m%d\033[m ƪ",
		x->userid, x->username, x->numlogins, x->numposts);
	hprintf("%s", buf);
	show_special_web(x->userid);//add by wjbta@bmy  ����id��ʶ
	printf("\n");
	hprintf("�ϴ��� [\033[1;32m%s\033[m] �� [\033[1;32m%s\033[m] ����վһ�Ρ�\n", ytht_ctime(x->lastlogin), x->lasthost);
	mails(userid, &tmp2);
	hprintf("���䣺[\033[1;32m%s\033[m]��", tmp2 ? "��" : "  ");
	if (!strcasecmp(x->userid, currentuser.userid)) {
		hprintf("����ֵ��[\033[1;32m%d\033[m](\033[33m%s\033[m) ", countexp(x), charexp(countexp(x)));
		hprintf("����ֵ��[\033[1;32m%d\033[m](\033[33m%s\033[m) ", countperf(x), cperf(countperf(x)));
	}
	hprintf("��������[\033[1;32m%d\033[m]��\n", count_life_value(x));
	if (x->userlevel & PERM_BOARDS) {
		hprintf("���ΰ���");
		sethomefile_s(filename, sizeof(filename), x->userid, "mboard");
		new_apply_record(filename, sizeof (struct boardmanager), (void *) bm_printboard, NULL);
		if (x->userlevel & !strcmp(x->userid, "SYSOP"))
			hprintf("[\033[1;36mϵͳ����Ա\033[m]");
		else if (x->userlevel & !strcmp(x->userid, "lanboy"))
			hprintf("[\033[1;36mϵͳ����Ա\033[m]");
		else if ((x->userlevel&PERM_SYSOP) && (x->userlevel&PERM_ARBITRATE) )
			hprintf("[\033[1;36m��վ������\033[m]");
		else if (x->userlevel & PERM_SYSOP)
			hprintf("[\033[1;36m����վ��\033[m]");
		else if (x->userlevel & PERM_OBOARDS)
			hprintf("[\033[1;36mʵϰվ��\033[m]");
		else if (x->userlevel & PERM_ARBITRATE)
			hprintf("[\033[1;36m���μ�ί\033[m]");
		else if (x->userlevel & PERM_SPECIAL4)
			hprintf("[\033[1;36m����\033[m]");
		else if (x->userlevel & PERM_WELCOME)
			hprintf("[\033[1;36mϵͳ����\033[m]");
		else if (x->userlevel & PERM_SPECIAL7) {
			if ( (x->userlevel & PERM_SPECIAL1) && !(x->userlevel & PERM_CLOAK) )
				hprintf("[\033[1;36m���γ���Ա\033[m]");
			else
				hprintf("[\033[1;36m�������Ա\033[m]");
		}
		else if (x->userlevel & PERM_ACCOUNTS)
			hprintf ("[\033[1;36m�ʺŹ���Ա\033[m]");
		hprintf("\n");
	}
	num = 0;
	for (i = 0; i < MAXACTIVE; i++) {
		u = ythtbbs_cache_utmp_get_by_idx(i);
		if (!strcmp(u->userid, x->userid)) {
			if (u->active == 0 || u->pid == 0 || (u->invisible && !HAS_PERM(PERM_SEECLOAK, currentuser)))
				continue;
			num++;
			if (num == 1)
				hprintf("Ŀǰ��վ��, ״̬����:\n");
			if (u->invisible)
				hprintf("\033[36mC\033[37m");
			if (u->mode != USERDF4)
				hprintf("\033[%dm%s\033[m ", u->pid == 1 ? 35 : 32, ModeType(u->mode));
			else
				hprintf("\033[%dm%s\033[m ", u->pid == 1 ? 35 : 32, u->user_state_temp); /* �Զ���״̬ */
			if (num % 5 == 0)
				printf("\n");
		}
	}
	if (num == 0) {
		hprintf("Ŀǰ����վ��, �ϴ���վʱ�� [\033[1;32m%s\033[m]\n\n",
				x->lastlogout ? ytht_ctime(x->lastlogout) :
				"�������ϻ��������߲���");
	}
	printf("\n");
	printf("</pre><table width=100%%><tr><td class=f2>");
	sethomefile_s(filename, sizeof(filename), x->userid, "plans");
	fp = fopen(filename, "r");
	sprintf(filename, "00%s-plan", x->userid);
	fdisplay_attach(NULL, NULL, NULL, NULL);
	if (fp) {
		while (1) {
			if (fgets(buf, 256, fp) == 0)
				break;
			if (!strncmp(buf, "begin 644 ", 10)) {
				errlog("old attach %s", filename);
				fdisplay_attach(stdout, fp, buf, filename);
				continue;
			}
			fhhprintf(stdout, "%s", buf);
		}
		fclose(fp);
	} else {
		hprintf("\033[36mû�и���˵����\033[37m\n");
	}
	printf("</td></tr></table>");
	printf("<br><br><a href=bbspstmail?userid=%s&title=û����>[�������]</a> ", x->userid);
	printf("<a href=bbssendmsg?destid=%s>[����ѶϢ]</a> ", x->userid);
	printf("<a href=bbsfadd?userid=%s>[�������]</a> ", x->userid);
	printf("<a href=bbsfdel?userid=%s>[ɾ������]</a>", x->userid);
	printf("<hr>");
	printf("<center><form action=bbsqry>\n");
	printf("�������û���: <input name=userid maxlength=12 size=12>\n");
	printf("<input type=submit value=��ѯ�û�>\n");
	printf("</form><hr>\n");
	printf("</body>\n");
	http_quit();
	return 0;
}

void
show_special(char *id2)
{
	FILE *fp;
	char id1[80], name[80];
	fp = fopen("etc/sysops", "r");
	if (fp == 0)
		return;
	while (1) {
		id1[0] = 0;
		name[0] = 0;
		if (fscanf(fp, "%s %s", id1, name) <= 0)
			break;
		if (!strcmp(id1, id2))
			hprintf(" \033[1;31m��\033[0;36m%s\033[1;31m��\033[m", name);
	}
	fclose(fp);
}

static int bm_printboard(struct boardmanager *bm, void *farg) {
	if (getboard(bm->board)){
		printf("<a href=%s%s target=f3>", showByDefMode(), bm->board);
		hprintf("%s", bm->board);
		printf("</a> ");
		hprintf(" ");
	}
	return 0;
}

