/*
	Pirate Bulletin Board System
	Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
	Eagles Bulletin Board System
	Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
						Guy Vega, gtvega@seabass.st.usm.edu
						Dominic Tynes, dbtynes@seabass.st.usm.edu
	Firebird Bulletin Board System
	Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
						Peng Piaw Foong, ppfoong@csie.ncu.edu.tw

	Copyright (C) 1999, KCN,Zhou Lin, kcn@cic.tsinghua.edu.cn

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 1, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#include "bbs.h"
#include "xyz.h"
#include "smth_screen.h"
#include "stuff.h"
#include "maintain.h"
#include "io.h"
#include "namecomplete.h"
#include "bcache.h"
#include "talk.h"
#include "main.h"
#include "mail.h"
#include "announce.h"
#include "bbsinc.h"
#include "boards.h"
#include "bbs_global_vars.h"

void
offline()
{
	char buf[STRLEN];

	modify_user_mode(OFFLINE);
	clear();
	if (HAS_PERM(PERM_SYSOP, currentuser) || HAS_PERM(PERM_BOARDS, currentuser) || HAS_PERM(PERM_ADMINMENU, currentuser) || HAS_PERM(PERM_SEEULEVELS, currentuser)) {
		move(1, 0);
		prints("\n\n������������, ���������ɱ��!!\n");
		pressreturn();
		clear();
		return;
	}
	if (currentuser.stay < 86400) {
		move(1, 0);

		prints("\n\n�Բ���, ����δ���ʸ�ִ�д�����!!\n");
		prints("ֻ����վʱ�䳬��24Сʱ���û�������ɱ.\n");
		pressreturn();
		clear();
		return;
	}

	getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !ytht_crypt_checkpasswd(currentuser.passwd, buf)) {
		prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "�������ʲô����? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname)) {
		prints("\n\n�ܱ�Ǹ, �Ҳ�����ʶ�㡣\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	prints("\033[1;5;31m����\033[0;1;31m ��ɱ��id�����ͷ�����û���κι�ϵ\033[m \n");
	prints("��ɱ������������id�޷�����ʹ�ã���id������ֻ���������й�ϵ\n");
	prints("������˸�������ɱ���԰������������е�id������ע���µ�id�����뿪����\n");
	prints("Ȼ��Ѹ�������˴�һ��\n\n");
	prints("\033[1;5;31m����\033[0;1;31m�� ��ɱ��, ������꽫���������������, Ը����Ϣ");
	prints("\n\n\n\033[1;32m���ѹ��.....\033[m\n\n\n");
	if (askyn("��ȷ��Ҫ�뿪������ͥ", NA, NA) == 1) {
		clear();
		set_safe_record();
		currentuser.dietime = currentuser.stay + 6*18 * 24 * 60 * 60;//����ɱ�ָ�Ϊ6*18��   �����ֻ�+18�����
		substitute_record(PASSFILE, &currentuser, sizeof (currentuser), usernum);
		Q_Goodbye();
		return;
	}
}

int
online()
{
	char buf[STRLEN];
	modify_user_mode(OFFLINE);
	clear();
	if ((currentuser.stay <= currentuser.dietime)) {
		move(1, 0);
		prints("\n\n����δ����!!\n");
		prints("��Ҫ�����˰�!\n");
		prints("������ڻ��� %d ����", 1 + (currentuser.dietime - currentuser.stay) / 60);
		pressreturn();
		clear();
		return -1;
	}
	getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !ytht_crypt_checkpasswd(currentuser.passwd, buf)) {
		prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
		pressreturn();
		clear();
		return -2;
	}
	getdata(3, 0, "�������ʲô����? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname)) {
		prints("\n\n�ܱ�Ǹ, �Ҳ�����ʶ�㡣\n");
		pressreturn();
		clear();
		return -3;
	}
	clear();
	move(1, 0);
	prints("\033[1;5;31m����\033[0;1;31m�� ����,���ǻ���,�Ǹ�ֵ�ÿ��ǵ�����");
	prints("\n\n\n\033[1;32m����Ҫ�����.....\033[m\n\n\n");
	if (askyn("��ȷ��Ҫ����Ϊ������?", NA, NA) == 1) {
		clear();
		currentuser.dietime = 0;
		substitute_record(PASSFILE, &currentuser, sizeof (currentuser), usernum);
		return Q_Goodbye();
	}
	return 0;
}

int
kick_user(const struct user_info *userinfo, int mode)
{
	int id, ind;
	struct user_info uin;
	struct userec kuinfo;
	char kickuser[40];
	char kickreason[STRLEN];
	char titlebuf[STRLEN];
	char contentbuf[STRLEN];
	char repbuf[STRLEN];
	char msgbuf[STRLEN * 2];
	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
		modify_user_mode(ADMIN);
		stand_title("Kick User");
		move(1, 0);
		usercomplete("Enter userid to be kicked: ", kickuser);
		if (*kickuser == '\0') {
			clear();
			return 0;
		}
		if (!(id = getuser(kickuser))) {
			move(3, 0);
			prints("Invalid User Id");
			clrtoeol();
			pressreturn();
			clear();
			return 0;
		}
		move(1, 0);
		prints("Kick User '%s'.", kickuser);
		clrtoeol();
		getdata(2, 0, "(Yes, or No) [N]: ", genbuf, 4, DOECHO, YEA);
		if (genbuf[0] != 'Y' && genbuf[0] != 'y') {	/* if not yes quit */
			move(2, 0);
			prints("Aborting Kick User\n");
			pressreturn();
			clear();
			return 0;
		}
		search_record(PASSFILE, &kuinfo, sizeof (kuinfo), (void *) cmpuids, kickuser);
		ind = search_ulist(&uin, t_cmpuids, id);
	} else {
		uin = *userinfo;
		strcpy(kickuser, uin.userid);
/*
		id = getuser(kickuser);
		search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);
		ind = search_ulist( &uin, t_cmpuids, id );
*/
		ind = YEA;
	}


	//���Լ�ʱ mode=1
	if(mode!=1)
	{
		clear();
		move(1,0);
		getdata(2,0,"����ԭ��: ", kickreason,STRLEN,DOECHO,YEA);
	} else {
		kickreason[0] = 0;
	}
	if (uin.pid != 1 && (!ind || !uin.active || uin.pid <= 0 || (kill(uin.pid, 0) == -1))) {
		if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
			move(3, 0);
			prints("User Has Logged Out");
			clrtoeol();
			pressreturn();
			clear();
		}
		return 0;
	} else if (kill(uin.pid, SIGHUP) < 0) {
		prints("User can't be kicked");
		pressreturn();
		clear();
		return 1;
	}
	sprintf(contentbuf, "%s",kickreason);
	sprintf(titlebuf,"%s��%s�߳�վ��", currentuser.userid, kickuser);
	securityreport(titlebuf,contentbuf);

	sprintf(repbuf,"����%sǿ���뿪��վ",currentuser.userid);
	snprintf(msgbuf, sizeof(msgbuf), "����:%s\n",kickreason);
	mail_buf(msgbuf,kickuser, repbuf);
	sprintf(genbuf, "%s kick %s", currentuser.userid, kickuser);
	newtrace(genbuf);
	move(2, 0);
	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
		prints("User has been Kicked\n");
		pressreturn();
		clear();
	}
	return 1;
}

