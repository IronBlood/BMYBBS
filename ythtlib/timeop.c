#include "ythtlib.h"
#include "time.h"

char *
Ctime(time_t clock)
{
	char *tmp;
	char *ptr = ctime(&clock);
	tmp = strchr(ptr, '\n');
	if (NULL != tmp)
		*tmp = 0;
	return ptr;
}

char * Difftime(time_t compared_time) {
	static char ret[64];
	time_t now = time(NULL);
	int now_i = (int)now;
	int tgt_i = (int)compared_time;

	int diff_i = abs(now_i - tgt_i);

	if(diff_i < 60) { //һ������
		sprintf(ret, "%d����%s", diff_i, (now_i>tgt_i) ? "ǰ" : "��");
	} else if (diff_i < 3600) { // һСʱ��
		sprintf(ret, "%d����%s", diff_i/60, (now_i>tgt_i) ? "ǰ" : "��");
	} else if (diff_i < 86400) { // һ������
		sprintf(ret, "%dСʱ%s", diff_i/3600, (now_i>tgt_i) ? "ǰ" : "��");
	} else { // ����һ��
		sprintf(ret, "%d��%s", diff_i/86400, (now_i>tgt_i) ? "ǰ" : "��");
	}

	return ret;
}

time_t get_time_of_the_biginning_of_the_day(struct tm *tm)
{
	// ����Ϊ UTC 0:00:00
	tm->tm_sec = 0;
	tm->tm_min = 0;
	tm->tm_hour = 0;

	return mktime(tm) - 8*3600; // ���� UTC+8 0:00:00
}
