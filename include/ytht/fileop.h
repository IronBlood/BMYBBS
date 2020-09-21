/* fileop.c */
#ifndef __FILEOP_H
#define __FILEOP_H
#include <time.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <fcntl.h>
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
extern sigjmp_buf bus_jump;

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

/**
 * A wrapper of stat(2)
 * @warning Thread UNsafe
 * @param file
 * @return
 */
struct stat *f_stat(char *file) __attribute__((deprecated));

/**
 * A wrapper of lstat(2)
 * @warning Thread UNsafe
 * @param file
 * @return
 */
struct stat *l_stat(char *file) __attribute__((deprecated));

/**
 * A thread-safe wrapper of stat(2)
 * @param s
 * @param file
 * @return
 */
struct stat *f_stat_s(struct stat *s, const char *file);

/**
 * A thread-safe wrapper of lstat(2)
 * @param s
 * @param file
 * @return
 */
struct stat *l_stat_s(struct stat *s, const char *file);

#define file_size(x) (f_stat(x)->st_size)
#define file_time(x) (f_stat(x)->st_mtime)
#define file_rtime(x) (f_stat(x)->st_atime)
//#define file_exist(x) (file_time(x)!=0)
#define file_exist(x) (access(x, F_OK)==0)
#define file_isdir(x) ((f_stat(x)->st_mode & S_IFDIR)!=0)
#define file_isfile(x) ((f_stat(x)->st_mode & S_IFREG)!=0)
#define lfile_isdir(x) ((l_stat(x)->st_mode & S_IFDIR)!=0)
#endif
