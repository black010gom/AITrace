#include "ai.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 로컬 모델을 가정한 단순 스텁 구현 */

AiModel *ai_create(const char *model_path) {
    AiModel *m = (AiModel*)calloc(1, sizeof(AiModel));
    if (!m) return NULL;
    if (model_path) {
        m->model_path = util_strdup(model_path);
    }
    m->is_loaded = 0;
    m->internal = NULL;
    return m;
}

int ai_load(AiModel *model) {
    if (!model || !model->model_path) return -1;
    /* TODO: 실제 로컬 모델 로딩(ONNX/TFLite/ggml 등) */
    model->is_loaded = 1;
    return 0;
}

void ai_free(AiModel *model) {
    if (!model) return;
    free(model->model_path);
    /* TODO: internal 핸들 해제 */
    free(model);
}

static SixW *sixw_alloc(void) {
    SixW *s = (SixW*)calloc(1, sizeof(SixW));
    return s;
}

/* 간단한 휴리스틱 기반 스텁: 실제 모델 추론 대체 */
int ai_infer_sixw(AiModel *model, const ConnectionInfo *conn, const char *location_hint, SixW **result_out) {
    if (!model || !model->is_loaded || !conn || !result_out) return -1;
    SixW *s = sixw_alloc();
    if (!s) return -1;

    /* who/when/where/what/how/why 생성 */
    s->who  = util_strdup("local user");
    s->when = util_format_time(conn->timestamp);
    s->where= util_strdup(location_hint ? location_hint : "unknown");
    /* what: 서비스 힌트 또는 포트 기반 추론 */
    char *service = NULL;
    if (conn->service_hint) {
        s->what = util_strdup(conn->service_hint);
    } else if (net_guess_service(conn->remote_port, &service) == 0 && service) {
        s->what = service; /* 소유권 이전 */
        service = NULL;
    } else {
        s->what = util_strdup("network activity");
    }
    /* how: 프로토콜 + 포트 */
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s via port %u", conn->protocol ? conn->protocol : "UNKNOWN", conn->remote_port);
        s->how = util_strdup(buf);
    }
    /* why: 간단 추정 */
    if (s->what && strcmp(s->what, "https") == 0) {
        s->why = util_strdup("secure web access");
    } else if (s->what && strcmp(s->what, "dns") == 0) {
        s->why = util_strdup("domain resolution");
    } else {
        s->why = util_strdup("data exchange");
    }

    *result_out = s;
    return 0;
}

void sixw_free(SixW *s) {
    if (!s) return;
    free(s->who);
    free(s->when);
    free(s->where);
    free(s->what);
    free(s->how);
    free(s->why);
    free(s);
}

/* 업데이트 체크/다운로드 스텁 */
int ai_check_update_available(void) {
    /* 오프라인이면 0, 온라인이면 원격 버전 비교 등 */
    /* TODO: 실제 버전 체크 */
    return 0;
}

int ai_download_update(const char *dest_model_path) {
    (void)dest_model_path;
    /* TODO: curl로 모델 파일 다운로드 및 교체 */
    return 0;
}
