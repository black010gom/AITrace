#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *util_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = (char*)malloc(len + 1);
    if (!p) return NULL;
    strcpy(p, s);
    return p;
}

char *util_format_time(time_t t) {
    char buf[64];
    struct tm *tmv = localtime(&t);
    if (tmv) {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmv);
        return util_strdup(buf);
    }
    return util_strdup("unknown");
}

int util_get_home(char **out_home) {
    if (!out_home) return -1;
#ifdef _WIN32
    const char *p = getenv("USERPROFILE");
#else
    const char *p = getenv("HOME");
#endif
    if (!p) return -1;
    *out_home = util_strdup(p);
    return *out_home ? 0 : -1;
}

int util_get_downloads(char **out_downloads_dir) {
    if (!out_downloads_dir) return -1;
    char *home = NULL;
    if (util_get_home(&home) != 0 || !home) return -1;
#ifdef _WIN32
    const char *suffix = "\\Downloads";
#else
    const char *suffix = "/Downloads";
#endif
    size_t len = strlen(home) + strlen(suffix) + 1;
    char *path = (char*)malloc(len);
    if (!path) { free(home); return -1; }
    strcpy(path, home);
    strcat(path, suffix);
    free(home);
    *out_downloads_dir = path;
    return 0;
}

int util_has_internet(void) {
    /* 간단한 체크 스텁: 실제로는 소켓으로 외부 도메인에 non-blocking 연결 테스트 등을 수행 */
    return 0; /* 0: 오프라인 가정 */
}
