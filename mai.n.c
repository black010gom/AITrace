#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "ai.h"
#include "csv.h"
#include "util.h"

int main(void) {
    /* 네트워크 초기화 */
    if (net_init() != 0) {
        fprintf(stderr, "Network init failed\n");
        return 1;
    }

    /* AI 모델 준비 (로컬 모델 경로 예시) */
    AiModel *ai = ai_create("Downloads/models/local_ai.bin");
    if (!ai || ai_load(ai) != 0) {
        fprintf(stderr, "AI model load failed\n");
        net_cleanup();
        if (ai) ai_free(ai);
        return 1;
    }

    /* 연결 목록 수집 */
    ConnList list;
    if (connlist_init(&list, 16) != 0) {
        fprintf(stderr, "ConnList init failed\n");
        ai_free(ai);
        net_cleanup();
        return 1;
    }

    /* TCP/UDP 모두 수집 */
    if (net_collect_active_tcp(&list) != 0) {
        fprintf(stderr, "Collect TCP failed (skeleton)\n");
    }
    if (net_collect_active_udp(&list) != 0) {
        fprintf(stderr, "Collect UDP failed (skeleton)\n");
    }

    /* CSV 파일 준비 */
    char *csv_path = NULL;
    if (csv_build_downloads_path(&csv_path) != 0 || !csv_path) {
        fprintf(stderr, "CSV path build failed\n");
        connlist_free(&list);
        ai_free(ai);
        net_cleanup();
        return 1;
    }
    FILE *fp = csv_open_write(csv_path);
    if (!fp) {
        fprintf(stderr, "CSV open failed: %s\n", csv_path);
        free(csv_path);
        connlist_free(&list);
        ai_free(ai);
        net_cleanup();
        return 1;
    }
    free(csv_path);

    csv_write_header(fp);

    /* 위치 힌트(예: 부산) */
    const char *location_hint = "Busan, KR";

    /* 각 연결에 대해 AI로 육하원칙 생성 후 CSV에 기록 */
    for (size_t i = 0; i < list.count; ++i) {
        ConnectionInfo *c = list.items[i];
        if (!c) continue;

        /* reverse DNS -> service hint 보강 (옵션) */
        char *hostname = NULL;
        if (net_reverse_dns(c->remote_ip, &hostname) == 0 && hostname) {
            /* 간단한 힌트: hostname에 따라 service_hint 없을 경우 채움 */
            if (!c->service_hint) {
                /* 예: *.google.* -> https 추정 등 (스텁) */
                c->service_hint = util_strdup("unknown");
            }
            free(hostname);
        }

        SixW *s = NULL;
        if (ai_infer_sixw(ai, c, location_hint, &s) == 0 && s) {
            csv_write_row(fp, c, s);
            sixw_free(s);
        }
    }

    csv_close(fp);
    connlist_free(&list);
    ai_free(ai);
    net_cleanup();

    printf("Done.\n");
    return 0;
}
