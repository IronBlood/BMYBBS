#include "bbslib.h"

void
printdiv(int *n, char *str)
{//modify by mintbaggio 040411 for new www, modify 041225
	printf("<tr><td align=right><img id=img%d src=\"/images/plus.gif\"></td>\n"
			"<td><DIV class=r id=div%da> <a class=linkleft href=\"javascript:;\" id=menu onClick=\"changemn('%d');\">%s</a>",
			*n, *n, *n, str);
	printf("<script type=\"text/javascript\">"
			"function clickRadio()"
			"  {"
			"document.getElementById('menu').click()\n"
			"  }"
			"</script>");

	if(!strcmp(str, "����������")){
		//modify: \"_bbsall.htm\" to: bbssecfly by: flyinsea
		printf("<a href=bbssecfly target=f3 style=\"font-size:8px;\"> &gt;&gt;</a> ");
	}
	printf("</div></td></tr>\n");
	if(!strcmp(str, "Ԥ��������"))
		printf("<tr><td colspan=2> <DIV class=s id=div%d>", (*n)++);
	else
		printf("<tr><td align=right></td><td> <DIV class=s id=div%d>", (*n)++);
}

void
printsectree(const struct sectree *sec)
{
	int i;
	for (i = 0; i < sec->nsubsec; i++) {
		printf("&nbsp;&nbsp;<a target=f3 href=boa?secstr=%s class=linkleft>"
				"%s%c</a><br>\n", sec->subsec[i]->basestr,
				nohtml(sec->subsec[i]->title),
				sec->subsec[i]->nsubsec ? '+' : ' ');
	}
}

int
bbsleft_main()
{
	int i;
	int div = 0;
	char buf[512];
	FILE* fp;
	changemode(MMENU);
	html_header(2);

	//modify by mintbaggio 040411 for new www
	printf("<script src=\"/inc/func.js\"></script>"
			"<script language=\"JavaScript\" type=\"text/JavaScript\">"
			"<!--\n"
			"\t	function MM_preloadImages() { //v3.0\n"
			"\t\t		var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();\n"
			"\t\t		var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)\n"
			"\t\t		if (a[i].indexOf(\"#\")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}\n"
			"\t	}\n"
			"-->\n"
			"</script>\n</head>\n"
			"<body class=\"level2\" leftmargin=0 topmargin=0 onMouseOver='doMouseOver()' onMouseEnter='doMouseOver()' onMouseOut='doMouseOut()' onload=\"clickRadio()\">\n");
	printf("<table width=100%% border=0 cellpadding=0 cellspacing=0>\n"
			"<tr><td width=100%% height=14></td></tr>\n"
			"<tr><td height=16 class=\"level2\">&nbsp;</td></tr>\n"
			"<tr><td height=70 class=\"level3\">\n");
	printf("<table width=100%% border=0 cellpadding=0 cellspacing=1>\n<tr><td><div align=center>\n");
	if (!loginok || isguest) {
		printf("<form action=bbslogin method=post target=_top name=loginform><tr><td>\n"
			 "<div style=\"text-align:center\">�û���¼</div>\n"
			 "�ʺ�<input type=text name=id maxlength=12 size=8 class=inputuser><br>\n"
			 "����<input type=password name=pw maxlength=12 size=8 class=inputpwd><br>\n"
			 "<input type=submit class=sumbitshort value=��¼>&nbsp;"
			 "<input type=submit class=resetshort value=ע�� onclick=\"{openreg();return false}\">"
			 "&nbsp&nbsp<a target=f3 href='bbsfindpass' target='_blank' class=linkindex><br>�һ��ʺŻ�����</a><br>\n"
			 "</form>\n");
	} else {
		char buf[256] = "δע���û�";
		printf("<a class=1100>�û�: <a href=bbsqry?userid=%s target=f3>%s</a><br>",
				currentuser.userid, currentuser.userid);
		if (currentuser.userlevel & PERM_LOGINOK)
			strcpy(buf, charexp(countexp(&currentuser)));
		if (currentuser.userlevel & PERM_BOARDS)
			strcpy(buf, "����");
		if (currentuser.userlevel & PERM_XEMPT)
			strcpy(buf, "�����ʺ�");
		if (currentuser.userlevel & PERM_SYSOP)
			strcpy(buf, "��վվ��");
		printf("<a class=1100>����: %s<br>", buf);
		printf("<a href=bbslogout target=_top class=1100>ע�����ε�¼</a><br>\n");
	}
	printf("</div></td></tr></table></td> </tr></table>\n");
	printf("<table width=100%% border=0 cellpadding=0 cellspacing=0>\n");
	printf("<tr><td width=27 align=right><img src=\"/images/list2.gif\"></td>\n"
			"<td width=107><a href=\"boa?secstr=?\" target=f3 class=linkleft>" MY_BBS_ID "����</a></td></tr>\n");
	printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			"<td><a href=bbs0an target=f3 class=linkleft>����������</a></td></tr>\n");
	printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			"<td><a href=\"bbstop10\" target=f3 class=linkleft><b>���Ż���ҳ</b></a></td></tr>\n");
	//Add by liuche 20121119 order by oOIOo ^_^
	printdiv(&div, "BMY��ʾǽ");
	printf("&nbsp;&nbsp;<a target=f3 href=gdoc?B=AcademicClub class=linkleft>������Ϣ</a><br>\n");
	printf("&nbsp;&nbsp;<a target=f3 href=gdoc?B=Activity class=linkleft>У԰�</a><br>\n");
	printf("&nbsp;&nbsp;<a class=linkleft href=\"%sLost_Found\" target=f3>ʧ������</a><br>\n", showByDefMode() );
	printf("&nbsp;&nbsp;<a class=linkleft href=\"tdoc?board=BoardHome\" target=f3>��Ҫ������</a><br>\n");
	printf("&nbsp;&nbsp;<a class=linkleft href=\"%sBMY_Dev\" target=f3>�����뱨��</a><br>\n", showByDefMode() );
	printf("&nbsp;&nbsp;<a class=linkleft href=\"%sArtDesign\" target=f3>�������վ</a><br>\n", showByDefMode() );
	printf("</div></td></tr>\n");

	if (loginok && !isguest) {
		char *ptr, buf[10];
		struct boardmem *x1;
		int mybrdmode;
		readuservalue(currentuser.userid, "mybrdmode", buf, sizeof (buf));
		mybrdmode = atoi(buf);
		printdiv(&div, "Ԥ��������");
		readmybrd(currentuser.userid);
		for (i = 0; i < mybrdnum; i++) {
			ptr = mybrd[i];
			if (!mybrdmode) {
				x1 = getboard(mybrd[i]);
				if (x1)
					ptr = nohtml(titlestr(x1->header.title));
			}
			printf("&nbsp;&nbsp;<a target=f3 href=%s%s class=linkleft>%s</a><br>\n", showByDefMode(), mybrd[i], ptr);
		}
		printf("&nbsp;&nbsp;<a target=f3 href=bbsboa?secstr=* class=linkleft>Ԥ��������</a><br>\n");
		printf("&nbsp;&nbsp;<a target=f3 href=bbsmybrd?mode=1 class=linkleft>Ԥ������</a><br>\n");
		printf("</div></td></tr>\n");
	}
	printdiv(&div, "����������");
	printsectree(&sectree);
	printf("</div></td></tr>\n");
	printdiv(&div, "̸��˵��");
	if (loginok && !isguest) {
		printf("&nbsp;&nbsp;<a href=bbsfriend target=f3 class=linkleft>���ߺ���</a><br>\n");
	}
	printf("&nbsp;&nbsp;<a href=bbsufind?search=A&limit=20 target=f3 class=linkleft>�����ķ�</a><br>\n");
	printf("&nbsp;&nbsp;<a href=bbsqry target=f3 class=linkleft>��ѯ����</a><br>\n");
	if (currentuser.userlevel & PERM_PAGE) {
		printf("&nbsp;&nbsp;<a href=bbssendmsg target=f3 class=linkleft>����ѶϢ</a><br>\n");
		printf("&nbsp;&nbsp;<a href=bbsmsg target=f3 class=linkleft>�鿴����ѶϢ</a><br>\n");
	}
	printf("</div></td></tr>\n");
	if (loginok && !isguest) {
		printdiv(&div, "���˹�����");
		printf("&nbsp;&nbsp;<a target=f3 href=bbsinfo class=linkleft>��������</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsplan class=linkleft>��˵����</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbssig class=linkleft>��ǩ����</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbspwd?mode=1 class=linkleft>�޸�����</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsparm class=linkleft>�޸ĸ��˲���</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsmywww class=linkleft>www���˶���</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsnick class=linkleft>��ʱ���ǳ�</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsstat class=linkleft>����ͳ��</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsfall class=linkleft>�趨����</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsball class=linkleft>�趨������</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsmybrd?mode=2 class=linkleft>RSS���Ĺ���</a><br>");
		if (currentuser.userlevel & PERM_CLOAK)
			printf("&nbsp;&nbsp;<a target=f3 "
					"onclick='return confirm(\"ȷʵ�л�����״̬��?\")' "
					"href=bbscloak class=linkleft>�л�����</a><br>\n");
		printf("</div></td></tr>");
		printdiv(&div, "�����ż�");
		printf("&nbsp;&nbsp;<a target=f3 href=bbsnewmail class=linkleft>���ʼ�</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsmail class=linkleft>�ռ���</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbsmail?box_type=1 class=linkleft>������</a><br>"
				"&nbsp;&nbsp;<a target=f3 href=bbspstmail class=linkleft>�����ʼ�</a><br>"
				"</div></td></tr>");
	}
	printdiv(&div, "�ر����");
	printf("&nbsp;&nbsp;<a target=f3 href=/wnl.html class=linkleft>������</a><br>\n");
	printf("&nbsp;&nbsp;<a target=f3 href=/scicalc.html class=linkleft>��ѧ������</a><br>\n");
	printf("&nbsp;&nbsp;<a href=bbsfind target=f3 class=linkleft>���²�ѯ</a><br>\n");

	printf("&nbsp;&nbsp;<a target=f3 href=bbsx?chm=0 class=linkleft>���ؾ�����</a><br>\n");

	printf("</div></td></tr>\n");
	printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			"<td><a class=linkleft href=\"bbs0an?path=/groups/GROUP_0/PersonalCorpus\" target=f3>�����ļ���</a>\n"
			"</td></tr>\n");
	printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			"<td><a class=linkleft href=\"bbsall\" target=f3>����������</a></td></tr>\n");
	printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			"<td><a class=linkleft href=\"bbsselstyle\" target=f3>��������</a>"
			"</td></tr>\n");
	printf("<tr><form action=bbssbs target=f3><td colspan=2>\n"
			"&nbsp;&nbsp;&nbsp;&nbsp;<input type=text name=keyword maxlength=20 "
			"size=9 onclick=\"this.select()\" value=ѡ��������><input type=submit class=sumbitgrey value=go></td></form></tr>\n");
			if (loginok && !isguest && !(currentuser.userlevel & PERM_LOGINOK) && !has_fill_form())
			printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
			 "<td><a class=linkleft href=\"bbsform\" target=f3>��дע�ᵥ</a></td></tr>\n");
		if (loginok && !isguest && HAS_PERM(PERM_SYSOP, currentuser))        //add by mintbaggio@BMY for www SYSOP kick www user
			printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
					"<td><a class=linkleft href=\"kick\" target=f3>��www��վ</a></td></tr>\n");
		printdiv(&div, "��������");
		printf("&nbsp;&nbsp;<a target=_BLANK href='http://www.xjtu.edu.cn/' class=linkleft>������ҳ</a><br>\n");
		printf("&nbsp;&nbsp;<a target=_BLANK href='http://nic.xjtu.edu.cn/' class=linkleft>��������</a><br>\n");
		printf("</div></td></tr>\n");
		printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
				"<td><a class=linkleft href=\"%sBBShelp\" target=f3>�û�����</a>\n"
				"</td></tr>\n", showByDefMode() );
		printf("<tr><td align=right> <img src=\"/images/list2.gif\"></td>\n"
				"<td><a class=linkleft href=\"bbspstmail?userid=SYSOP\" target=f3>���Ÿ�վ��</a>\n"
				"</td></tr>\n");
		printf("</table>\n");
		printf("<table width=124>"
				"<tr><form><td>&nbsp;&nbsp;<input type=button style='width:90px' value='���ز˵�' "
				"onclick=\"{if(switchAutoHide()==true) {this.value='ֹͣ�Զ�����';}"
				"else this.value='���ز˵�';return false;}\">"
				"</td></form></tr></table>\n");

		fp = fopen("etc/ad_left", "r");
		if(!fp){
			//printf("fail to open\n");
			goto endleft;
		}
		bzero(buf, 512);
		while(fgets(buf, 512, fp)){
			ytht_strtrim(buf);
			if (strlen(buf) <= 1)
				continue;
			char *p = strchr(buf, ' ');
			if (p == NULL)
				continue;
			*p = '\0';

			printf("<table width=124>"
					"<tr><td>"
					"<a href='%s' target='_blank'><img src='%s' width=120 height=44 border=0 /></a>"
					"</td></tr></table>\n", p+1, buf);
		}
		fclose(fp);

endleft:

		// end of interma announce log picture

		printf("<script>if(isNS4) arrange();if(isOP)alarrangeO();</script>");
		//add by macintosh 20051216
		if (loginok && !isguest) {
			printf("<script>\n"
					"window.onbeforeunload = function(){\n"
					"  if(event.clientX>document.body.clientWidth&&event.clientY<0||event.altKey){\n"
					"return 'ֱ�ӹر��������������վʱ�䣬ǿ�ҽ������㡰ע�����ε�¼����'}}\n"
					"</script>\n");
			//add by macintosh 20051216, end
			if (HAS_PERM(PERM_LOGINOK, currentuser) && !HAS_PERM(PERM_POST, currentuser))
				printf("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�sysop�湫��, ��������sysop��������. ��������, ����committee���������.')</script>\n");
			mails(currentuser.userid, &i);
			if (i > 0)
				printf("<script>alert('�������ż�!')</script>\n");
		}
		if (isguest && 0)
			printf("<script>setTimeout('open(\"regreq\", \"winREGREQ\", \"width=600,height=460\")', 1800000);</script>");
		if (loginok && !isguest) {
			char filename[80];
			sethomepath(filename, currentuser.userid);
			mkdir(filename, 0755);
			sethomefile(filename, currentuser.userid, BADLOGINFILE);
			if (file_exist(filename)) {
				printf("<script>"
						"window.open('bbsbadlogins', 'badlogins', 'toolbar=0, scrollbars=1, location=0, statusbar=1, menubar=0, resizable=1, width=450, height=300');"
						"</script>");
			}
		}
		if (!via_proxy && wwwcache->accel_port && wwwcache->accel_ip)
			printf("<script src=http://%s:%d/testdoc.js></script>",
					inet_ntoa(wwwcache->accel_addr), wwwcache->accel_port);
		else if (via_proxy)
			printf("<script src=/testdoc.js></script>");
		printf("</body></html>");


	printf("</body></html>");
return 0;
}

