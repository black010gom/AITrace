#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
int net_init(void) {
    WSADATA wsa;
    return (WSAStartup(MAKEWORD(2,2), &wsa) == 0) ? 0 : -1;
}
void net_cleanup(void) { WSACleanup(); }

/* Windows에서는 GetExtendedTcpTable/UDPTable 등을 사용할 수 있음.
   여기서는 뼈대: netstat -n 호출 결과를 파싱하는 단순 예시 형태로 남김. */
static int parse_netstat_output(ConnList *list, const char *proto_cmd) {
    /* TODO: _popen("netstat -n") 등으로 라인 파싱하여 conn_create/append */
    (void)list; (void)proto_cmd;
    return 0; /* 구현 필요 */
}

int net_collect_active_tcp(ConnList *list) { return parse_netstat_output(list, "TCP"); }
int net_collect_active_udp(ConnList *list) { return parse_netstat_output(list, "UDP"); }

#else

int net_init(void) { return 0; }
void net_cleanup(void) {}

/* Linux/Unix: /proc/net/tcp, /proc/net/udp 파싱 */
static int parse_proc_net(ConnList *list, const char *path, const char *proto) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    char line[1024];
    /* 첫 줄 헤더 스킵 */
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return -1; }
    while (fgets(line, sizeof(line), fp)) {
        /* 매우 간단한 파서 뼈대: 실제로는 hex ip:port를 파싱해야 함 */
        /* TODO: 실제 구현 - local_address, rem_address, st 등 파싱 */
        /* 여기서는 예시로 더미 한 줄을 만들지 않고 스켈레톤만 유지 */
        (void)list; (void)proto;
    }
    fclose(fp);
    return 0;
}

int net_collect_active_tcp(ConnList *list) { return parse_proc_net(list, "/proc/net/tcp", "TCP"); }
int net_collect_active_udp(ConnList *list) { return parse_proc_net(list, "/proc/net/udp", "UDP"); }

#endif

int net_reverse_dns(const char *ip, char **hostname_out) {
    if (!ip || !hostname_out) return -1;
    struct in_addr addr;
    if (inet_pton(AF_INET, ip, &addr) != 1) return -1;
    struct hostent *he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he && he->h_name) {
        *hostname_out = (char*)malloc(strlen(he->h_name) + 1);
        if (!*hostname_out) return -1;
        strcpy(*hostname_out, he->h_name);
        return 0;
    }
    *hostname_out = NULL;
    return 0;
}

int net_guess_service(uint16_t port, char **service_out) {
    if (!service_out) return -1;
    const char *s = NULL;
    switch (port) {
        case 80:  s = "http"; break;
        case 443: s = "https"; break;
        case 53:  s = "dns"; break;
        case 25:  s = "smtp"; break;
        case 110: s = "pop3"; break;
        case 995: s = "pop3s"; break;
        case 143: s = "imap"; break;
        case 993: s = "imaps"; break;
        default:  s = "unknown"; break;
    }
    *service_out = (char*)malloc(strlen(s) + 1);
    if (!*service_out) return -1;
    strcpy(*service_out, s);
    return 0;
}

int connlist_init(ConnList *list, size_t initial_capacity) {
    if (!list) return -1;
    list->count = 0;
    list->capacity = initial_capacity ? initial_capacity : 8;
    list->items = (ConnectionInfo**)calloc(list->capacity, sizeof(ConnectionInfo*));
    return list->items ? 0 : -1;
}

int connlist_append(ConnList *list, ConnectionInfo *conn) {
    if (!list || !conn) return -1;
    if (list->count == list->capacity) {
        size_t newcap = list->capacity * 2;
        ConnectionInfo **tmp = (ConnectionInfo**)realloc(list->items, newcap * sizeof(ConnectionInfo*));
        if (!tmp) return -1;
        list->items = tmp;
        list->capacity = newcap;
    }
    list->items[list->count++] = conn;
    return 0;
}

void connlist_free(ConnList *list) {
    if (!list) return;
    for (size_t i = 0; i < list->count; ++i) {
        conn_free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = list->capacity = 0;
}

ConnectionInfo *conn_create(const char *ip, uint16_t port, const char *proto, const char *service_hint) {
    ConnectionInfo *c = (ConnectionInfo*)calloc(1, sizeof(ConnectionInfo));
    if (!c) return NULL;
    if (ip) {
        c->remote_ip = (char*)malloc(strlen(ip) + 1);
        if (!c->remote_ip) { free(c); return NULL; }
        strcpy(c->remote_ip, ip);
    }
    if (proto) {
        c->protocol = (char*)malloc(strlen(proto) + 1);
        if (!c->protocol) { free(c->remote_ip); free(c); return NULL; }
        strcpy(c->protocol, proto);
    }
    if (service_hint) {
        c->service_hint = (char*)malloc(strlen(service_hint) + 1);
        if (!c->service_hint) { free(c->protocol); free(c->remote_ip); free(c); return NULL; }
        strcpy(c->service_hint, service_hint);
    }
    c->remote_port = port;
    c->timestamp = time(NULL);
    return c;
}

void conn_free(ConnectionInfo *conn) {
    if (!conn) return;
    free(conn->remote_ip);
    free(conn->protocol);
    free(conn->service_hint);
    free(conn);
}
