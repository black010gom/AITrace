#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include "net.h"
#include "ai.h"

/* 다운로드 폴더의 CSV 경로 생성 */
int csv_build_downloads_path(char **out_path);

/* CSV 파일 열기/닫기 */
FILE *csv_open_write(const char *path);
void csv_close(FILE *fp);

/* 헤더 라인 작성 */
int csv_write_header(FILE *fp);

/* ConnectionInfo + SixW 기록 */
int csv_write_row(FILE *fp, const ConnectionInfo *conn, const SixW *s);

#endif /* CSV_H */
