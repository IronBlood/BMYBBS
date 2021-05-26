#include "bbslib.h"
#include "bmy/article.h"

int
bbssnd_main()
{
	char filename[80], dir[80], board[32], title[80], buf[256], userid[20] ,*content, *ref, *content0;
	int r, sig, mark = 0, outgoing, anony, guestre = 0, usemath, nore, mailback;
	int is1984, to1984 = 0;
	size_t i;
	struct boardmem *brd;
	struct fileheader *x = NULL;
	int thread = -1;
	struct mmapfile mf = { .ptr = NULL };
	html_header(1);

	ytht_strsncpy(board, getparm("board"), sizeof(board));
	ytht_strsncpy(title, getparm("title"), 60);
	ytht_strsncpy(userid, getparm("replyto"), 14);
	outgoing = strlen(getparm("outgoing"));
	anony = strlen(getparm("anony"));
	usemath = strlen(getparm("usemath"));
	nore = strlen(getparm("nore"));
	mailback = strlen(getparm("mailback"));
	brd = getboard(board);
	if (brd == 0)
		http_fatal("���������������");
	strcpy(board, brd->header.filename);
	sprintf(dir, "boards/%s/.DIR", board);
	ref = getparm("ref");
	r = atoi(getparm("rid"));
	thread = atoi(getparm("th"));
	if (ref[0]) {
		MMAP_TRY {
			if (mmapfile(dir, &mf) == -1) {
				MMAP_UNTRY;
				http_fatal("�����������");
			}
			x = findbarticle(&mf, ref, &r, 1);
		}
		MMAP_CATCH {
		}
		MMAP_END mmapfile(NULL, &mf);

		if (x == NULL) {
			http_fatal("���²�����");
		}

		if (x->accessed & FH_NOREPLY)
			http_fatal("���ı���Ϊ����Reģʽ");

		if (x && (x->accessed & FH_ALLREPLY)) {
			if (strncmp(x->title, "Re: ", 4))
				snprintf(title, sizeof(title), "Re: %s", x->title);
			else
				snprintf(title, sizeof(title), "%s", x->title);
			guestre = 1;
			mark = mark | FH_ALLREPLY;
		}
		if (x)
			thread = x->thread;
	} else {
		thread = -1;
	}
	if(strcmp(board, "welcome") && strcmp(board, "KaoYan")){	//add by mintbaggio 040614 for post at "welcome" + "KaoYan"(by wsf)
		if (!loginok || (isguest && !guestre))
			http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
	}
	else if (seek_in_file(MY_BBS_HOME"/etc/guestbanip", fromhost) && (!guestre || !loginok) )
		http_fatal("����ip����ֹʹ��guest�ڱ��淢������!��");
	changemode(POSTING);

	if (!(brd->header.flag & ANONY_FLAG))
		anony = 0;
	if (brd->header.flag & IS1984_FLAG)
		is1984 = 1;
	else
		is1984 = 0;
	for (i = 0; i < strlen(title); i++)
		if (title[i] <= 27 && title[i] >= -1)
			title[i] = ' ';
	i = strlen(title) - 1;
	while (i > 0 && isspace(title[i]))
		title[i--] = 0;
	sig = atoi(getparm("signature"));
	currentuser.signature = sig;
	content = getparm("text");
	content0 = strdup(content);
	//http_fatal("just for test");
	if (usemath && bmy_article_include_math(content))
		mark |= FH_MATH;
	if (nore)
		mark |= FH_NOREPLY;
	if (mailback)
		mark |= FH_MAILREPLY;
	if (title[0] == 0)
		http_fatal("���±���Ҫ�б���");
	if(strcmp(board, "welcome") && strcmp(board, "KaoYan")){   //add by mintbaggio 040614 for post at "welcome" + "KaoYan"(by wsf)
		if (!has_post_perm(&currentuser, brd) && !guestre)
			http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������.");
	}
	if (noadm4political(board))
		http_fatal("�Բ���,��Ϊû�а��������Ա����,������ʱ���.");

	if ((now_t - w_info->lastposttime) < 6) {
		w_info->lastposttime = now_t;
		http_fatal("���η��ļ������, ����Ϣ���������");
	}
	w_info->lastposttime = now_t;
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	f_write(filename, content);
	if (!ythtbbs_board_is_hidden_x(brd)) {
		enum ytht_smth_filter_result dangerous = dofilter(title, filename, ythtbbs_board_is_political(board) ? YTHT_SMTH_FILTER_OPTION_NORMAL : YTHT_SMTH_FILTER_OPTION_SIMPLE);
		if (dangerous == YTHT_SMTH_FILTER_RESULT_1984){
			to1984 = 1;
			mail_file(filename, currentuser.userid, title, currentuser.userid);
		}else if (dangerous == YTHT_SMTH_FILTER_RESULT_WARN) {
			char mtitle[256];
			sprintf(mtitle, "[������] %s %.60s", board, title);
			post_mail("delete", mtitle, filename,
					currentuser.userid, currentuser.username,
					fromhost, -1, 0);
			ythtbbs_cache_Board_updatelastpost("deleterequest");
			mark |= FH_DANGEROUS;
		}
	}

	if (userid[0])
		post_mail(userid, title, filename, currentuser.userid, currentuser.username, currentuser.lasthost, sig-1, 0);

	if (insertattachments(filename, content, currentuser.userid))
		mark = mark | FH_ATTACHED;

	if (is1984 || to1984) {
		r = post_article_1984(board, title, filename,
				currentuser.userid, currentuser.username,
				fromhost, sig - 1, mark, outgoing, thread);
	} else if (anony)
		r = post_article(board, title, filename, "Anonymous",
				"����������ʹ", "������ʹ�ļ�", 0, mark,
				outgoing, currentuser.userid, thread);
	else
		r = post_article(board, title, filename, currentuser.userid,
				currentuser.username, fromhost, sig - 1, mark,
				outgoing, currentuser.userid, thread);
	if (r <= 0)
		http_fatal("�ڲ������޷�����");
	if (!is1984 && !to1984) {
		brc_initial(currentuser.userid, board);
		brc_add_readt(r);
		brc_update(currentuser.userid);
	}
	unlink(filename);
	sprintf(buf, "%s post %s %s", currentuser.userid, board, title);
	newtrace(buf);
	if (brd->header.clubnum ==0 && !junkboard(board)) {
		currentuser.numposts++;
		save_user_data(&currentuser);
	}
	if (to1984) {
		printf("%s<br>\n", BAD_WORD_NOTICE);
		printf("[<a href='javascript:history.go(-2)'>����</a>]");
	} else {
		sprintf(buf, "%s%s", showByDefMode(), board);
		redirect(buf);
	}

	// ���ͻ������ѿ�ʼ by IronBlood
	char noti_userid[14] = { '\0' };
	if(x!=NULL && r>0 && strchr(x->owner, '.') == NULL) { // x ��Ϊ�գ����ظ�ģʽ���������ɹ�������Ϊ��վ�û�
		if(x->owner[0] == '\0') { // �����û�
			memcpy(noti_userid, &x->owner[1], IDLEN);
		} else {
			memcpy(noti_userid, x->owner, IDLEN);
		}
		if(strcmp(currentuser.userid, noti_userid) != 0) { // �����û��͵�ǰ�û�����ͬ��ʱ��
			add_post_notification(noti_userid, (anony) ? "Anonymous" : currentuser.userid, board, r, title);
		}
	} // ���ͻ������ѽ���

	// ���� @ ���ѿ�ʼ by IronBlood
	char mention_ids[MAX_MENTION_ID][IDLEN+2];
	memset(mention_ids, 0, MAX_MENTION_ID*(IDLEN+2));
	parse_mentions(content0, (char **)mention_ids, 1);
	struct userec *ue;
	i=0;
	while(i!=MAX_MENTION_ID && mention_ids[i][0] != 0) {
		// ��Ϊmention_ids ���ܴ�Сд�ͱ�վ�û� ID ��ͬ����Ҫ���⴦��
		ue = getuser(mention_ids[i]);
		if(ue!=NULL && strcmp(currentuser.userid, ue->userid)!=0 && has_read_perm(ue, board) && !ythtbbs_override_included(ue->userid, YTHTBBS_OVERRIDE_REJECTS, currentuser.userid))
			add_mention_notification(ue->userid, (anony) ? "Anonymous" : currentuser.userid, board, r, title);
		++i;
	}
	// ���� @ ���ѽ���
	free(content0);
	return 0;
}
