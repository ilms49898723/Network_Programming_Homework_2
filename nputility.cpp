#include "nputility.h"

void setSocketTimeout(const int& socketfd, const int& second, const int& millisecond) {
    timeval tv;
    tv.tv_sec = second;
    tv.tv_usec = millisecond * 1000;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void trimNewLine(char* src) {
    size_t len = strlen(src);
    if (src[len - 1] == '\n') {
        src[len - 1] = '\0';
    }
}

int stringHash(const char* src, size_t len) {
    int coef = 31;
    int ret = 0;
    for (size_t i = 0; i < len; ++i) {
        ret = ret * coef + src[i];
        coef *= 31;
    }
    return ret;
}

ConnectInfo getConnectInfo(const sockaddr_in& sock) {
    ConnectInfo ret;
    ret.address = inet_ntoa(sock.sin_addr);
    ret.port = ntohs(sock.sin_port);
    return ret;
}

