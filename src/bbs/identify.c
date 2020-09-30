#include "bbs.h"
#include "ythtbbs/identify.h"
#include "smth_screen.h"
#include "io.h"
#include "maintain.h"
#include "namecomplete.h"
#include "stuff.h"
#include "bcache.h"

#ifdef POP_CHECK

int x_active_manager();
static int query_active(char* userid);
static int force_comfirm(char* userid);
static int delete_active(char* userid);
static int update_active(char* userid);
static int query_value(char* value, int style);

//��֤��Ϣ��������
int x_active_manager()
{
    char an[2];
//	char style[2];
    char userid[IDLEN+2];
    char value[VALUELEN];
    if (!HAS_PERM(PERM_ACCOUNTS) && !HAS_PERM(PERM_SYSOP)) {
        clear();
        move(2, 0);
        prints("��û�й���Ȩ��!");
        return 0;
    }
    clear();
 INPUT:   
    stand_title("ʵ����֤����ѡ��\n\n");
    clrtobot();
    move(3, 0);
    prints("[1] ��������֤��ǿ�м���ĳ�û�\n");
    prints("[2] ��ѯʵ����֤��¼\n");
    prints("[3] �޸�ʵ����֤��¼(��������֤״̬)\n");
    prints("[4] ɾ��ʵ����֤��¼(�ͷ�����)\n");
    prints("[5] ��ѯĳ��¼�°󶨵�id\n");
    prints("[6] �뿪");


    getdata(10 ,0, ">> ", an,2,DOECHO ,YEA);


    if (!strcmp(an, "1")) {
        clear();
        move(2, 0);
        prints("����Ҫ�����id: ");
        usercomplete(" ", userid);
	if (*userid)    force_comfirm(userid);
       goto INPUT;
    } else if (!strcmp(an, "2")) {
        clear();
        move(1, 0);
        prints("����Ҫ��ѯ��id: ");
        usercomplete(" ", userid);
        if (*userid) query_active(userid);
        goto INPUT;
    } else if (!strcmp(an, "3")) {
        clear();
        move(1, 0);
        prints("����Ҫ�޸ĵ�id: ");
        usercomplete(" ", userid);
        if (*userid) update_active(userid);
        goto INPUT;
    } else if (!strcmp(an, "4")) {
        clear();
        move(1, 0);
        prints("����Ҫ�����֤��id: ");
	getdata(3, 0, ">> ", userid, VALUELEN, DOECHO, YEA);
        if (*userid) delete_active(userid);
        goto INPUT;
    } else if (!strcmp(an, "5")) {
        clear();
        move(1, 0);
	 prints("����Ҫ��ѯ��%s:\n", style_to_str(MAIL_ACTIVE));
	 getdata(3, 0, ">> ", value, VALUELEN, DOECHO, YEA);
        if (*value) query_value(value, MAIL_ACTIVE);
        goto INPUT;
    } 
    return 0;
}

//��ѯĳid����֤��Ϣ
static int query_active(char* userid)
{
    struct active_data act_data;
    char value[VALUELEN];
    unsigned int i;
    struct associated_userid *au;

    i=read_active(userid, &act_data);
    getuser(userid);

    if (i>0) {
		ytht_str_to_lowercase(act_data.email);
	 //if (act_data.status==MAIL_ACTIVE && !strcmp(strstr(act_data.email, "@")+1, "mails.gucas.ac.cn")) {
	//	char gname[VALUELEN];
	//	char gdept[VALUELEN];
	//	u2g(act_data.name, strlen(act_data.name), gname, VALUELEN);
	//	u2g(act_data.dept, strlen(act_data.dept), gdept, VALUELEN);
	//	strcpy(act_data.name, gname);
	//	strcpy(act_data.dept, gdept);
       // }
        clear();
        move(1, 0);
        prints("�û���   :\t%s\n", act_data.userid);
        prints("����     :\t%s\n", *act_data.name?act_data.name:lookupuser.realname);
        prints("%s����     :\t%s\n", act_data.status==1?"\033[31m":"\033[37m", act_data.email);
        prints("%s�绰     :\t%s\n", act_data.status==2?"\033[31m":"\033[37m", act_data.phone);
        prints("%s���֤�� :\t%s\n", act_data.status==3?"\033[31m":"\033[37m", act_data.idnum);
        prints("ѧ��     :\t%s\n", act_data.stdnum);
        prints("������λ :\t%s\n", *act_data.dept?act_data.dept:lookupuser.realmail);
	 prints("��ַ     :\t%s\n", lookupuser.address);
        prints("��֤ʱ�� :\t%s\n", act_data.status<1?"N/A":act_data.uptime);
        prints("��֤���� :\t%s\n", style_to_str(act_data.status));
	prints("����id   :\t%s\n", act_data.operator);
        if (act_data.status==IDCARD_ACTIVE) {
            //��ʾͼƬ��ַ
        }
        if (act_data.status>0 && act_data.status<4) {
            get_active_value(value, &act_data);
            prints("\n----------------------------------------------------------\n\n");
            prints("ͬ��֤��¼�µ�����ID:\n");
            au = get_associated_userid_by_style(act_data.status, value);
            //�г�ͬ��¼�µ�����id
            if (au != NULL) {
				for (i = 0; i < au->count; ++i) {
					prints("%-12s\t%s\n", au->id_array[i], style_to_str(au->status_array[i]));
				}
				free_associated_userid(au);
			}
        }
    } else {
        move(5, 0);
        prints("δ�ҵ��û� %s ����֤�����Ϣ!", userid);
    }
    pressreturn();
    return 1;		
}

//ǿ�Ƽ���ĳ�û�
static int force_comfirm(char* userid)
{
    struct userec cuser;
    struct active_data act_data;
    char an[2];
    char genbuf[STRLEN];
    int response;

    response=getuser(userid);
    //memset(&act_data, 0, sizeof(struct active_data));
    read_active( userid, &act_data);

    if (lookupuser.userlevel& PERM_LOGINOK) {
        clear();
        move(5, 0);
        prints("���û��Ѿ�����!\n");
        pressreturn();
        return 0;
    }
	clear();
    move(5, 0);
    prints("ȷ������?\n");
    getdata(8, 0, "Y/N [N] >> ", an, 2, DOECHO, YEA);
    if (*an == 'Y' || *an == 'y') {
        strcpy(act_data.userid, userid);
	 strcpy(act_data.operator, currentuser.userid);
	 act_data.status=FORCE_ACTIVE;
	 strcpy(act_data.ip, currentuser.lasthost);
	 write_active(&act_data);
	
	memcpy(&cuser, &lookupuser, sizeof (lookupuser));
	cuser.userlevel |= PERM_DEFAULT;	// by ylsdd
	substitute_record(PASSFILE, &cuser, sizeof (struct userec), response);

	// lookupuser.userlevel |= PERM_DEFAULT;
	 //�����¼û��д
	 sprintf(genbuf, "%s��%sǿ��ͨ����֤.", currentuser.userid, lookupuser.userid);
	 securityreport(genbuf, genbuf);
        pressreturn();
        return 1;
    }
    return 0;
}

//ɾ�������¼
static int delete_active(char* userid)
{
//	struct userec* cuser;
    char an[2];
    char genbuf[STRLEN];
    struct active_data act_data;

    getuser(userid);
    read_active(userid, &act_data);
   // s = mysql_init(s);
   /*
    if (!(cuser->flags & ACTIVATED_FLAG)) {
        clrtobot();
        move(5, 0);
        prints("���û���δ����!\n");
        //anyway����Ȼȥ�������¼
        act_data.status=NO_ACTIVE;
        strcpy(act_data.operator, getCurrentUser()->userid);
        write_active(&act_data);
        setactivefile(genbuf, cuser->userid, ACTIVE_FILE);
        //ɾ���û�Ŀ¼�µ���֤���ļ�
        unlink(genbuf);
        pressreturn();
        return 0;
    }
    */
    clear();
    move(5, 0);
    prints("ȷ��ȡ����֤��¼?\n");
    getdata(6, 0, "Y/N [N] >> ", an, 2, DOECHO, YEA);
    if (*an == 'Y' || *an == 'y') {
        act_data.status=NO_ACTIVE;
        strcpy(act_data.operator, currentuser.userid);
        write_active(&act_data);
        //cuser->flags &= ~ACTIVATED_FLAG;
        //setactivefile(genbuf, cuser->userid, ACTIVE_FILE);
        //ɾ���û�Ŀ¼�µ���֤���ļ�
        //unlink(genbuf);
	 //�����¼û��д
	 sprintf(genbuf, "%sɾ��%s�������.", currentuser.userid, lookupuser.userid);
	 securityreport(genbuf, genbuf);
        pressreturn();
        return 1;
    }
    return 0;
}

static void
getfield(line, info, desc, buf, len)
int line, len;
char *info, *desc, *buf;
{
	char prompt[STRLEN];
	char genbuf[STRLEN];

	sprintf(genbuf, "  ԭ���趨: %-46.46s [1;32m(%s)[m",
		(buf[0] == '\0') ? "(δ�趨)" : buf, info);
	move(line, 0);
	prints("%s", genbuf);
	sprintf(prompt, "  %s: ", desc);
	getdata(line + 1, 0, prompt, genbuf, len, DOECHO, YEA);
	if (genbuf[0] != '\0') {
		strncpy(buf, genbuf, len);
	}
	move(line, 0);
	clrtoeol();
	prints("  %s: %s\n", desc, buf);
	clrtoeol();
}

//���¼�����Ϣ��¼
//һ�������Ӧ��ɾ�������¼��Ҫ���û������ֹ�����
//������ʹ�ñ�����
static int update_active(char* userid)
{
//    struct userec* cuser;
    struct active_data act_data;
	char genbuf[STRLEN];
    char an[2];
	int response;

    getuser(userid);
    response=read_active(userid, &act_data);

    move(4,0);
    clear();
    getfield(5, "", "��ʵ����", act_data.name, STRLEN);
    getfield(6, "", "������λ", act_data.dept, STRLEN);
    getfield(7, "", "Email", act_data.email, STRLEN);
    getfield(8, "", "�ֻ���", act_data.phone, STRLEN);
    getfield(9, "", "���֤������", act_data.idnum, STRLEN);
    getfield(10, "", "ѧ��", act_data.stdnum, STRLEN);
	ytht_str_to_lowercase(act_data.email);
    prints("ȷ������?\n");
    getdata(12, 0, "Y/N [N] >> ", an, STRLEN, DOECHO, YEA);
    if (*an == 'Y' || *an == 'y') {
        //��¼����վ���id
        act_data.status=FORCE_ACTIVE;
	 strcpy(act_data.ip, currentuser.lasthost);
	 strcpy(act_data.operator, currentuser.userid);
	 //�����¼û��д
	 sprintf(genbuf, "%s�޸�%s��ʵ����֤��¼.", currentuser.userid, lookupuser.userid);
	 securityreport(genbuf, genbuf);
	 write_active(&act_data);
        pressreturn();
        return 1;
    }
    return 0;
}


//��ѯĳ��¼�°󶨵�id
static int query_value(char* value, int style)
{
    size_t i;
    struct associated_userid *au;

    clear();
    move(5, 0);
	ytht_str_to_lowercase(value);
    prints("ͬ��֤��¼�µ�ID:\n");
    au = get_associated_userid_by_style(style, value);
    //�г�ͬ��¼�µ�����id
    if (au != NULL) {
		for (i = 0; i < au->count; ++i) {
			prints("%-12s\t%s\n", au->id_array[i], style_to_str(au->status_array[i]));
		}
		free_associated_userid(au);
	}
    pressreturn();
    return 1;
}
#endif
