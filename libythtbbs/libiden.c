#include "bbs.h"
#include "identify.h"
#include "mysql_wrapper.h"

#ifdef POP_CHECK

static const char *active_style_str[] = {"", "email", "phone", "idnum", NULL};
const char *MAIL_DOMAINS[] = {"", "stu.xjtu.edu.cn", "mail.xjtu.edu.cn", "idp.xjtu6.edu.cn", NULL};
const char *IP_POP[] = {"", "202.117.1.22", "202.117.1.28", "2001:250:1001:2::ca75:1c0", NULL};

static void convert_mysql_time_to_str(char *buf, MYSQL_TIME *mt) {
snprintf(buf, 20,
		 "%04d-%02d-%02d %02d:%02d:%02d",
		 mt->year, mt->month, mt->day,
		 mt->hour, mt->minute, mt->second);
}

//�ж������Ƿ�Ϸ�
int invalid_mail(char* mbox)
{
	if (strstr(mbox, "@bbs.")) return 1;
	if (strstr(mbox, ".bbs@")) return 1;
	if (!strstr(mbox, "@")) return 1;
	//if (invalidaddr(mbox)) return 0;
	if (strcmp(mbox+strlen(mbox)-5, "ac.cn")!=0) return 1;
	return 0;
}

/*
//���������
void gencode(char* code)
{
    int i;
    int c1, c2;
    for (i=0; i<CODELEN; ++i) {
        c1=rand()%2;
        c2=rand()%26;
        code[i]=65+c1*32+c2;
    }
    code[CODELEN]='\0';
}
*/

char* str_to_uppercase(char *str)
{
	char *h = str;
	while (*str != '\n' && *str != 0) {
		*str = toupper(*str);
		str++;
	}
	return h;
}

char* str_to_lowercase(char *str)
{
	char *h = str;
	while (*str != '\n' && *str != 0) {
		*str = tolower(*str);
		str++;
	}
	return h;
}


const char* style_to_str(int style)
{
	switch (style) {
	case NO_ACTIVE:
		return "δ��֤";
		break;
	case MAIL_ACTIVE:
		return "����";
		break;
	case PHONE_ACTIVE:
		return "�ֻ�����";
		break;
	case IDCARD_ACTIVE:
		return "����֤��";
		break;
	case FORCE_ACTIVE:
		return "�ֹ�����";
		break;
	default:
		return "δ֪";
	}
}

/*
//������֤�ź�
int send_active_mail(char* mbox, char* code, char* userid, session_t* session)
{
    int return_no;
    FILE *fout;
    char buff[STRLEN];
    char mtitle[STRLEN];
    //char acturl[STRLEN];
    const char *c=sysconf_str("BBS_WEBDOMAIN");

    if (!c) c=sysconf_str("BBSDOMAIN");
    sethomefile(buff, userid, "active_mail");
    fout = fopen(buff, "w");
    if (fout != NULL) {
        fprintf(fout, "Reply-To: bbs@%s\n", "kyxk.net");
        fprintf(fout, "From: bbs@%s\n",  "kyxk.net");
        fprintf(fout, "To: %s\n", str_to_lowercase(mbox));
        fprintf(fout, "Subject: %sʵ����֤ȷ���ź�.\n", BBS_FULL_NAME);
        fprintf(fout, "X-Forwarded-By: SYSOP \n");
        fprintf(fout, "X-Disclaimer: None\n");
        fprintf(fout, "\n");
        fprintf(fout,"���ã�����%s��ʵ������֤ȷ���ź��������û���ڱ�վע�ᣬ�벻����᱾�ż�.\n", BBS_FULL_NAME);
        fprintf(fout,"��ע���ʹ���ߴ����ǣ�%s\n\n",userid);
        fprintf(fout,"����ʵ����֤��֤���ǣ�\n%s\n\n",code);
        // fprintf(fout,"������ֱ�ӵ���������������֤������\n");
        // fprintf(fout,"%s\n\n", acturl);
        fprintf(fout,"���Ʊ���֤�벢�ڱ�վ��֤���������룬���������֤����\n\n");
        fprintf(fout,"�װ��� %s, %s��ӭ���Ĺ���!",userid, BBS_FULL_NAME);
        fprintf(fout, ".\n");
        fclose(fout);
        sprintf(mtitle, "%s��ӭ��!", BBS_FULL_NAME);
        return_no = bbs_sendmail(buff,  mtitle, str_to_lowercase(mbox), 0, 1, session);
        unlink(buff);
        return return_no;
    }

    return 0;
}

int send_active_msg(char* phone, char* code,char* userid)
{
    char command[512];
    char sqlbuf[512];
    MYSQL* s=NULL;
    MYSQL_RES *res;

    s=mysql_init(s);
    s = mysql_real_connect(s,"localhost",SQLUSER,SQLPASSWD,SQLDB,3306, NULL,0);
    sprintf(command, "%s. Welcome to KYXK BBS. Your verification code is:%s", userid, code);
    sprintf(sqlbuf,"INSERT INTO outbox(number, text) VALUES('%s', '%s'); " ,phone, command);
    mysql_real_query(s, sqlbuf, strlen(sqlbuf));
    res = mysql_store_result(s);
    mysql_close(s);

	//system(command);
    return 0;
}

//��֪��Ϊʲô�����ز��Ե�ʱ��sethomefile()��λ�����û���homeĿ¼
//���ʵ���Ͽ��ԵĻ��������װ������������
int setactivefile(char* genbuf, char* userid, char* filename)
{
    char fpath[STRLEN];
//    FILE* dp;

    strcpy(genbuf, BBS_HOMEPATH);
    strcat(genbuf, "/");
    sethomefile(fpath,userid,filename);
    strcat(genbuf, fpath);

    // sethomefile(genbuf,userid,filename);
    return 0;
}


//�û��ļ�Ŀ¼��д�������
int set_active_code(char* userid, char* code, char* value, int style)
{
    char genbuf[STRLEN];
    FILE* dp;

    setactivefile(genbuf,userid,ACTIVE_FILE);
    if ((dp=fopen(genbuf,"w"))==NULL) {
        fclose(dp);
        return 0;
    }
    fwrite(code, sizeof(char), CODELEN+1, dp);
    fwrite(value, sizeof(char), VALUELEN, dp);
    fwrite(&style, sizeof(int), 1, dp);
    fclose(dp);
    return 1;
}

//�û��ļ�Ŀ¼��д�������
//value��styleһ������Ǳ���url��֤��ʽʹ��
int get_active_code(char* userid, char* code, char* value, int* style)
{
    char genbuf[STRLEN];
    char fpath[STRLEN];
    FILE* dp;

    strcpy(fpath, BBS_HOMEPATH);
    strcat(fpath, "/");
    setactivefile(genbuf,userid,ACTIVE_FILE);
    strcat(fpath, genbuf);
    if (access(genbuf, 0)==0) {
        if ((dp=fopen(genbuf,"r"))==NULL) {
            fclose(dp);
            return 0;
        }
        fread(code, sizeof(char), CODELEN+1, dp);
        fread(value, sizeof(char), VALUELEN, dp);
        fread(style, sizeof(int), 1, dp);
        fclose(dp);
    } else {
        return FILE_NOT_FOUND;
    }
    return 1;
}
*/

int get_active_value(char* value, struct active_data* act_data)
{
	if (act_data->status==0) {
		return 0;
	}
	else if (act_data->status==MAIL_ACTIVE) {
		strcpy(value, act_data->email);
	}
	else if (act_data->status==PHONE_ACTIVE) {
		strcpy(value, act_data->phone);
	}
	else if (act_data->status==IDCARD_ACTIVE) {
		strcpy(value, act_data->idnum);
	}
	return 1;
}

static void query_record_num_callback(MYSQL_STMT *stmt, MYSQL_BIND *result_col, void *result_set) {
	mysql_stmt_fetch(stmt);
}

//��ѯĳ����¼���˼���id
int query_record_num(char* value, int style)
{
	char sqlbuf[512];
	char *str;
	MYSQL_BIND params[1], results[1];
	int status;
	char count[8];

	memset(count, 0, sizeof(count));
	str = strdup(value);
	str_to_lowercase(str);
	memset(params, 0, sizeof(params));
	memset(results, 0, sizeof(results));

	params[0].buffer_type = MYSQL_TYPE_STRING;
	params[0].buffer = str;
	params[0].buffer_length = strlen(str);

	results[0].buffer_type = MYSQL_TYPE_STRING;
	results[0].buffer = count;
	results[0].buffer_length = sizeof(count);

	sprintf(sqlbuf,"SELECT count(*) FROM %s WHERE %s=? AND status>0;", USERREG_TABLE, active_style_str[style]);
	status = execute_prep_stmt(sqlbuf, params, results, NULL, query_record_num_callback);

	free(str);
	return (status != MYSQL_OK) ? -1 : atoi(count);
}

/**
 * �������ļ���ȡ sql ��������Ϣ���ú������� mysql_real_connect �ķ�װ��
 * �������ļ���������������ʱ������ MYSQL* ָ�롣
 * @param s
 * @return
 */
__attribute__((deprecated)) MYSQL * my_connect_mysql(MYSQL *s)
{
	const char *MYSQL_CONFIG_FILE = MY_BBS_HOME "/etc/mysqlconfig";

	FILE *cfg_fp;
	int   cfg_fd;
	char  sql_user[16];
	char  sql_pass[16];
	char  sql_db[24];
	char  sql_port[8];
	char  sql_host[32];

	cfg_fp = fopen(MYSQL_CONFIG_FILE, "r");
	if (!cfg_fp)
		return NULL;

	cfg_fd = fileno(cfg_fp);
	flock(cfg_fd, LOCK_SH);

	readstrvalue_fp(cfg_fp, "SQL_USER", sql_user, sizeof(sql_user));
	readstrvalue_fp(cfg_fp, "SQL_PASS", sql_pass, sizeof(sql_pass));
	readstrvalue_fp(cfg_fp, "SQL_DB", sql_db, sizeof(sql_db));
	readstrvalue_fp(cfg_fp, "SQL_PORT", sql_port, sizeof(sql_port));
	readstrvalue_fp(cfg_fp, "SQL_HOST", sql_host, sizeof(sql_host));

	flock(cfg_fd, LOCK_UN);
	fclose(cfg_fp);
	return mysql_real_connect(s, sql_host, sql_user, sql_pass, sql_db, atoi(sql_port), NULL, CLIENT_IGNORE_SIGPIPE);
}

static void write_active_callback_count(MYSQL_STMT *stmt, MYSQL_BIND *result_cols, void *result_set) {
	mysql_stmt_fetch(stmt);
}

static void write_active_callback_I_U(MYSQL_STMT *stmt, MYSQL_BIND *result_cols, void *result_set) {
	*(int *) result_set = mysql_affected_rows(stmt->mysql);
}

/*д�����ݿ�
 */
int write_active(struct active_data* act_data)
{
	char sqlbuf[512];
	char count_s[8];
	int count, rc, row_affected;
	MYSQL_BIND params_count[1], params_I_U[10], results[1];

	memset(params_count, 0, sizeof(params_count));
	memset(params_I_U, 0, sizeof(params_I_U));
	memset(results, 0, sizeof(results));
	memset(count_s, 0, sizeof(count_s));
	rc = MYSQL_OK;
	row_affected = 0;

	sprintf(sqlbuf,"SELECT count(*) FROM %s WHERE userid=?;", USERREG_TABLE);
	params_count->buffer_type = MYSQL_TYPE_STRING;
	params_count->buffer = act_data->userid;
	params_count->buffer_length = strlen(act_data->userid);
	results->buffer_type = MYSQL_TYPE_STRING;
	results->buffer = count_s;
	results->buffer_length = sizeof(count_s);

	rc = execute_prep_stmt(sqlbuf, params_count, results, NULL, write_active_callback_count);
	if (rc != MYSQL_OK)
		return WRITE_FAIL;

	count = atoi(count_s);
	if (count == 0) {
		sprintf(sqlbuf,
			"INSERT INTO %s(name, ip, regtime, updatetime, operator, email, phone, idnum, studnum, dept, status, userid) "
			"VALUES(?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, ?, ?, ?, ?, ?, ?, ?, ?);", USERREG_TABLE);
	} else{
		sprintf(sqlbuf,
			"UPDATE %s SET updatetime=CURRENT_TIMESTAMP, name=?, ip=?, operator=?, email=?, phone=?, idnum=?, "
			"studnum=?, dept=?, status=? WHERE userid=?;", USERREG_TABLE);
	}

	params_I_U[0].buffer = act_data->name;
	params_I_U[0].buffer_length = strlen(act_data->name);
	params_I_U[0].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[1].buffer = act_data->ip;
	params_I_U[1].buffer_length = strlen(act_data->ip);
	params_I_U[1].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[2].buffer = act_data->operator;
	params_I_U[2].buffer_length = strlen(act_data->operator);
	params_I_U[2].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[3].buffer = act_data->email;
	params_I_U[3].buffer_length = strlen(act_data->email);
	params_I_U[3].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[4].buffer = act_data->phone;
	params_I_U[4].buffer_length = strlen(act_data->phone);
	params_I_U[4].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[5].buffer = act_data->idnum;
	params_I_U[5].buffer_length = strlen(act_data->idnum);
	params_I_U[5].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[6].buffer = act_data->stdnum;
	params_I_U[6].buffer_length = strlen(act_data->stdnum);
	params_I_U[6].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[7].buffer = act_data->dept;
	params_I_U[7].buffer_length = strlen(act_data->dept);
	params_I_U[7].buffer_type = MYSQL_TYPE_STRING;

	params_I_U[8].buffer = &(act_data->status);
	params_I_U[8].buffer_length = sizeof(int);
	params_I_U[8].buffer_type = MYSQL_TYPE_LONG;

	params_I_U[9].buffer = act_data->userid;
	params_I_U[9].buffer_length = strlen(act_data->userid);
	params_I_U[9].buffer_type = MYSQL_TYPE_STRING;

	rc = execute_prep_stmt(sqlbuf, params_I_U, NULL, &row_affected, write_active_callback_I_U);
	return (rc != MYSQL_OK || row_affected != 1) ? WRITE_FAIL : ((count == 0) ? WRITE_SUCCESS : UPDATE_SUCCESS);
}

static void read_active_callback(MYSQL_STMT *stmt, MYSQL_BIND *result_col, void *result_set) {
	*(int *)result_set = mysql_stmt_num_rows(stmt);
	if (*(int *)result_set > 0) {
		mysql_stmt_fetch(stmt);
	}
}

int read_active(char* userid, struct active_data* act_data)
{
	char sqlbuf[128];
	int count, rc;
	MYSQL_BIND params[1], results[11];
	MYSQL_TIME regtime, uptime;

	strncpy(act_data->userid, userid, IDLEN);
	sprintf(sqlbuf,"SELECT name, dept, ip, regtime, updatetime, operator, email, phone, idnum, studnum, status FROM %s WHERE userid=?;", USERREG_TABLE);

	memset(params, 0, sizeof(params));
	memset(results, 0, sizeof(results));

	params[0].buffer_type = MYSQL_TYPE_STRING;
	params[0].buffer = act_data->userid;
	params[0].buffer_length = strlen(act_data->userid);

	results[0].buffer_type = MYSQL_TYPE_STRING;
	results[0].buffer = act_data->name;
	results[0].buffer_length = STRLEN;

	results[1].buffer_type = MYSQL_TYPE_STRING;
	results[1].buffer = act_data->dept;
	results[1].buffer_length = STRLEN;

	results[2].buffer_type = MYSQL_TYPE_STRING;
	results[2].buffer = act_data->ip;
	results[2].buffer_length = 20;

	results[3].buffer_type = MYSQL_TYPE_TIMESTAMP;
	results[3].buffer = &regtime;
	results[3].buffer_length = sizeof(MYSQL_TIME);

	results[4].buffer_type = MYSQL_TYPE_TIMESTAMP;
	results[4].buffer = &uptime;
	results[4].buffer_length = sizeof(MYSQL_TIME);

	results[5].buffer_type = MYSQL_TYPE_STRING;
	results[5].buffer = act_data->operator;
	results[5].buffer_length = IDLEN+2;

	results[6].buffer_type = MYSQL_TYPE_STRING;
	results[6].buffer = act_data->email;
	results[6].buffer_length = VALUELEN;

	results[7].buffer_type = MYSQL_TYPE_STRING;
	results[7].buffer = act_data->phone;
	results[7].buffer_length = VALUELEN;

	results[8].buffer_type = MYSQL_TYPE_STRING;
	results[8].buffer = act_data->idnum;
	results[8].buffer_length = VALUELEN;

	results[9].buffer_type = MYSQL_TYPE_STRING;
	results[9].buffer = act_data->stdnum;
	results[9].buffer_length = VALUELEN;

	results[10].buffer_type = MYSQL_TYPE_INT24;
	results[10].buffer = &(act_data->status);
	results[10].buffer_length = sizeof(int);

	rc = execute_prep_stmt(sqlbuf, params, results, &count, read_active_callback);
	convert_mysql_time_to_str(act_data->regtime, &regtime);
	convert_mysql_time_to_str(act_data->uptime, &uptime);

	return (rc != MYSQL_OK) ? 0 : count;
}

static void get_associated_userid_callback(MYSQL_STMT *stmt, MYSQL_BIND *result_cols, void *result_set) {
	struct associated_userid **au = result_set;
	size_t idx;
	size_t rows;

	char *userid;
	long *user_status;

	*au = NULL;
	rows = mysql_stmt_num_rows(stmt);
	if (rows == 0) return;

	userid = result_cols[0].buffer;
	user_status = (long *) result_cols[1].buffer;

	*au = (struct associated_userid *) calloc(1, sizeof(struct associated_userid));
	(*au)->count = rows;
	(*au)->id_array = (char**) calloc(rows, sizeof(void *));
	(*au)->status_array = (int *) calloc(rows, sizeof(int));

	for (idx=0; idx < rows; idx++) {
		mysql_stmt_fetch(stmt);
		(*au)->id_array[idx] = strdup(userid);
		(*au)->status_array[idx] = (*user_status) % 10;
	}
}

struct associated_userid *get_associated_userid(const char *email) {
	char *sqlbuf;
	char userid[IDLEN + 1];
	long user_status;
	MYSQL_BIND params[1], results[2];
	struct associated_userid *au;

	user_status = 0;
	sqlbuf = "SELECT userid, status FROM " USERREG_TABLE " WHERE email=?;";

	memset(params, 0, sizeof(params));
	memset(results, 0, sizeof(results));

	params[0].buffer_type = MYSQL_TYPE_STRING;
	params[0].buffer = (void *)email;
	params[0].buffer_length = strlen(email);

	results[0].buffer_type = MYSQL_TYPE_STRING;
	results[0].buffer = (void *)userid;
	results[0].buffer_length = IDLEN;

	results[1].buffer_type = MYSQL_TYPE_LONG;
	results[1].buffer = (void *) &user_status;
	results[1].buffer_length = sizeof(long);

	au = NULL;
	execute_prep_stmt(sqlbuf, params, results, (void *) &au, get_associated_userid_callback);
	return au;
}

void free_associated_userid(struct associated_userid *au) {
	size_t i;
	if (au == NULL) return;

	for (i = 0; i < au->count; i++) {
		if (au->id_array[i])
			free(au->id_array[i]);
	}

	if (au->count > 0) {
		free(au->id_array);
		free(au->status_array);
	}

	free(au);
}

/*
int valid_stunum(char* mbox, char* stunum)
{
    char sqlbuf[512];
//    char stunum_cmp[32];
    MYSQL *s = NULL;
    int count;
    MYSQL_RES *res;
    MYSQL_ROW row;

    s = mysql_init(s);
    if (!my_connect_mysql(s)) {
        return 0;
    }
    sprintf(sqlbuf,"SELECT * FROM %s WHERE lower(email)='%s' AND studnum='%s'; " , SCHOOLDATA_TABLE, mbox, stunum);
    mysql_real_query(s, sqlbuf, strlen(sqlbuf));
    res = mysql_store_result(s);
    row=mysql_fetch_row(res);
//    strcpy(stunum_cmp, row[0]);
    count=mysql_num_rows(res);
    mysql_close(s);
//    return !strcmp(stunum, stunum_cmp);
    return count;
}

int get_official_data(struct active_data* act_data)
{
    char sqlbuf[512];
    MYSQL *s = NULL;
    MYSQL_RES *res;
    MYSQL_ROW row;

    s = mysql_init(s);
    if (!my_connect_mysql(s)) {
        return 0;
    }

    sprintf(sqlbuf,"SELECT name, dept  FROM %s WHERE lower(email)='%s' AND studnum='%s'; " , SCHOOLDATA_TABLE, str_to_lowercase(act_data->email), act_data->stdnum);
    mysql_real_query(s, sqlbuf, strlen(sqlbuf));
    res = mysql_store_result(s);
    row=mysql_fetch_row(res);
    strcpy(act_data->name, row[0]);
    strcpy(act_data->dept, row[1]);
    mysql_close(s);
    return 0;
}

*/

#endif

