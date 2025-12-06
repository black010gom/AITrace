#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <time.h>

/* 문자열 유틸(포인터 중심) */
char *util_strdup(const char *s);
char *util_format_time(time_t t);

/* 홈 경로/다운로드 폴더 */
int util_get_home(char **out_home);
int util_get_downloads(char **out_downloads_dir);

/* 네트워크 유틸: 인터넷 연결 여부 */
int util_has_internet(void);

#endif /* UTIL_H */
