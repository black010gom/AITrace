#ifndef NET_H
#define NET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <time.h>

/* 연결 정보 구조체 */
typedef struct ConnectionInfo {
    char *remote_ip;            /* 예: "203.0.113.45" */
    uint16_t remote_port;       /* 예: 443 */
    char *protocol;             /* "TCP" or "UDP" */
    char *service_hint;         /* 예: "https", "dns" 등 추론 힌트 */
    time_t timestamp;           /* 수집 시각 */
} ConnectionInfo;

/* 동적 배열 (이중포인터 기반) */
typedef struct ConnList {
    ConnectionInfo **items;     /* 이중포인터: 요소 포인터들의 배열 */
    size_t count;               /* 현재 개수 */
    size_t capacity;            /* 할당 크기 */
} ConnList;

/* 네트워크 초기화/종료 (플랫폼 별) */
int net_init(void);
void net_cleanup(void);

/* 연결 수집(플랫폼 별 구현) */
int net_collect_active_tcp(ConnList *list);
int net_collect_active_udp(ConnList *list);

/* 도메인/서비스 역추적 헬퍼 */
int net_reverse_dns(const char *ip, char **hostname_out);
int net_guess_service(uint16_t port, char **service_out);

/* ConnList 메모리 관리 */
int connlist_init(ConnList *list, size_t initial_capacity);
int connlist_append(ConnList *list, ConnectionInfo *conn);
void connlist_free(ConnList *list);

/* ConnectionInfo 생성/해제 */
ConnectionInfo *conn_create(const char *ip, uint16_t port, const char *proto, const char *service_hint);
void conn_free(ConnectionInfo *conn);

#endif /* NET_H */
