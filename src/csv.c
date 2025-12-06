#include "csv.h"
#include "util.h"
#include <stdlib.h>

int csv_build_downloads_path(char **out_path) {
    if (!out_path) return -1;
    char *downloads = NULL;
    if (util_get_downloads(&downloads) != 0 || !downloads) return -1;
    const char *filename = "/ip_log.csv";
    size_t len = strlen(downloads) + strlen(filename) + 1;
#ifdef _WIN32
    filename = "\\ip_log.csv";
    len = strlen(downloads) + strlen(filename) + 1;
#endif
    char *path = (char*)malloc(len);
    if (!path) { free(downloads); return -1; }
    strcpy(path, downloads);
    strcat(path, filename);
    *out_path = path;
    free(downloads);
    return 0;
}

FILE *csv_open_write(const char *path) {
#ifdef _WIN32
    return fopen(path, "w");
#else
    return fopen(path, "w");
#endif
}

void csv_close(FILE *fp) {
    if (fp) fclose(fp);
}

int csv_write_header(FILE *fp) {
    if (!fp) return -1;
    return fprintf(fp, "IP,Port,Protocol,Service,Time,Who,When,Where,What,How,Why\n") < 0 ? -1 : 0;
}

int csv_write_row(FILE *fp, const ConnectionInfo *conn, const SixW *s) {
    if (!fp || !conn || !s) return -1;
    char timebuf[64];
    struct tm *tmv = localtime(&conn->timestamp);
    if (tmv) {
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tmv);
    } else {
        strcpy(timebuf, "unknown");
    }
    return fprintf(fp, "%s,%u,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                   conn->remote_ip ? conn->remote_ip : "",
                   conn->remote_port,
                   conn->protocol ? conn->protocol : "",
                   conn->service_hint ? conn->service_hint : "",
                   timebuf,
                   s->who ? s->who : "",
                   s->when ? s->when : "",
                   s->where ? s->where : "",
                   s->what ? s->what : "",
                   s->how ? s->how : "",
                   s->why ? s->why : "") < 0 ? -1 : 0;
}
