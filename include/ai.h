#ifndef AI_H
#define AI_H

#include <stddef.h>
#include "net.h"

/* 육하원칙 결과 구조체 */
typedef struct SixW {
    char *who;
    char *when;
    char *where;
    char *what;
    char *how;
    char *why;
} SixW;

/* AI 모델 핸들 (로컬 모델 가정) */
typedef struct AiModel {
    void *internal;       /* 로더/런타임 핸들 (추상 포인터) */
    char *model_path;     /* 로컬 모델 파일 경로 */
    int   is_loaded;
} AiModel;

/* AI 초기화/로딩/해제 */
AiModel *ai_create(const char *model_path);
int ai_load(AiModel *model);
void ai_free(AiModel *model);

/* 오프라인 추론: IP/서비스 기반 육하원칙 생성 */
int ai_infer_sixw(AiModel *model, const ConnectionInfo *conn, const char *location_hint, SixW **result_out);

/* SixW 메모리 해제 */
void sixw_free(SixW *s);

/* 온라인 업데이트(필요 시) */
int ai_check_update_available(void);
int ai_download_update(const char *dest_model_path);

#endif /* AI_H */
