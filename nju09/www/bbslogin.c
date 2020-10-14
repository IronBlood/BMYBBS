#include "bbslib.h"
#include "ytht/random.h"
#include "bmy/cookie.h"

int bbslogin_main() {
	int rc;
	char buf[128], id[20], pw[20];
	struct user_info info;
	struct bmy_cookie cookie;

	ytht_strsncpy(id, getparm("id"), 13);
	ytht_strsncpy(pw, getparm("pw"), 13);

	if (loginok && strcasecmp(id, currentuser.userid) && !isguest) {
		http_fatal("ϵͳ��⵽Ŀǰ��ļ�������Ѿ���¼��һ���ʺ� %s�������˳�.(ѡ������logout)", currentuser.userid);
	}

	if (!strcmp(id, "")) {
		strcpy(id, "guest");
	}

	rc = ythtbbs_user_login(id, pw, fromhost, YTHTBBS_LOGIN_NJU09, &info, &currentuser, NULL);
	if (rc != YTHTBBS_USER_LOGIN_OK) {
		switch(rc) {
		case YTHTBBS_USER_NOT_EXIST:
			http_fatal("�����ʹ�����ʺ�");
			break;
		case YTHTBBS_USER_WRONG_PASSWORD:
			http_fatal("�������");
			break;
		case YTHTBBS_USER_SUSPENDED:
			http_fatal("���ʺ��ѱ�ͣ��, ��������, ���������ʺ���sysop��ѯ��.");
			break;
		case YTHTBBS_USER_SITE_BAN:
			http_fatal("�Բ���, ��վ����ӭ���� [%s] �ĵ�¼. <br>��������, ����SYSOP��ϵ.", fromhost);
			break;
		case YTHTBBS_USER_USER_BAN:
			http_fatal("��ID�����ý�ֹ��%s��¼", fromhost);
			break;
		case YTHTBBS_USER_TOO_FREQUENT:
			http_fatal("���ε�¼�������!");
			break;
		case YTHTBBS_USER_IN_PRISON:
			http_fatal("���ĸ��죬��Ҫ����");
			break;
		case YTHTBBS_USER_ONLINE_FULL:
			http_fatal("��Ǹ��Ŀǰ�����û����Ѵ����ޣ��޷���¼�����Ժ�������");
			break;
		default:
			http_fatal("unknown");
			break;
		}
	}

	if (strcasecmp(id, "guest")) {
		if (!readuservalue(info.userid, "wwwstyle", buf, sizeof (buf)))
			wwwstylenum = atoi(buf);
		if (wwwstylenum < 0 || wwwstylenum >= NWWWSTYLE)
			wwwstylenum = 1;
	} else {
		wwwstylenum = 1;
	}

	cookie.userid = info.userid;
	cookie.sessid = info.sessionid;
	cookie.token  = info.token;
	cookie.extraparam = getextrparam_str(wwwstylenum);
	bmy_cookie_gen(buf, sizeof(buf), &cookie);

	printf("Set-Cookie: " SMAGIC "=%s; SameSite=Strict; HttpOnly;", buf);
	html_header(3);
	redirect("/" SMAGIC "/"); // URL ���ٸ��� session ��Ϣ
	http_quit();
	return 0;
}

