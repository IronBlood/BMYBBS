/* fileop.c */
#ifndef __FILEOP_H
#define __FILEOP_H
#include <setjmp.h>
struct mmapfile {
	char *ptr;
	time_t mtime;
	size_t size;
};

#define MMAP_TRY \
    if (!sigsetjmp(bus_jump, 1)) { \
        signal(SIGBUS, sigbus);

#define MMAP_CATCH \
    } \
    else { \

#define MMAP_END } \
    signal(SIGBUS, SIG_IGN);

#define MMAP_UNTRY {signal(SIGBUS, SIG_IGN);}
#define MMAP_RETURN(x) {signal(SIGBUS, SIG_IGN);return (x);}
#define MMAP_RETURN_VOID {signal(SIGBUS, SIG_IGN);return;}
sigjmp_buf bus_jump;

int crossfs_rename(const char *oldpath, const char *newpath);
int readstrvalue(const char *filename, const char *str, char *value, int size);

/**
 * �������ļ��ж�ȡֵ
 * �����ļ���ʽ��
 *   key1 value1
 *   key2 value2
 * ���ú� readstrvalue һ��������������ͬʱ��һ���ļ��ж�ȡ������Ϣ������ fopen/fclose �ĵ��ô�����
 * @see readstrvalue
 * @param[in]   fp    �����ļ�������
 * @param[in]   str   ��
 * @param[out]  value ���ֵ�Ļ�����
 * @param[in]   size  ��������С
 * @return
 */
int readstrvalue_fp(FILE *fp, const char *str, char *value, size_t size);
int savestrvalue(const char *filename, const char *str, const char *value);
void sigbus(int signo);
int mmapfile(char *filename, struct mmapfile *pmf);
int trycreatefile(char *path, char *fnformat, int startnum, int maxtry);
int copyfile(char *source, char *destination);
int openlockfile(const char *filename, int flag, int op);
int checkfilename(const char *filename);
int clearpath(const char *path);

/**
 * @brief ���ļ��в����Ƿ����ĳ���ַ�����
 * �ú��������� src/talk.c �Լ� nju09/bbssnd.c��
 * @param filename �ļ�·������
 * @param seekstr ��Ҫ���ҵ��ַ���
 * @return ���������� 1�����򷵻� 0��
 */
int seek_in_file(char* filename, char *seekstr);
#endif
