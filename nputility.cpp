#include "nputility.h"

void udpSendTo(const int& socketfd, const char* msg, const size_t n, sockaddr*& dstSocketp) {
    socklen_t dstLen = sizeof(*dstSocketp);
    char buffer[MAXN];
    int byteSend, byteRecv;
    if ((byteSend = sendto(socketfd, msg, n, 0, dstSocketp, dstLen)) < 0) {
        fprintf(stderr, "udpSendTo: sendto: %s\n", strerror(errno));
        return;
    }
    while (true) {
        byteRecv = recvfrom(socketfd, buffer, MAXN, 0, dstSocketp, &dstLen);
        if (byteRecv < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                if ((byteSend = sendto(socketfd, msg, n, 0, dstSocketp, dstLen)) < 0) {
                    fprintf(stderr, "udpSendTo: sendto: %s\n", strerror(errno));
                    return;
                }
            }
            else {
                fprintf(stderr, "udpSendTo: recvfrom: %s\n", strerror(errno));
            }
        }
        else {
            break;
        }
    }
}

void udpRecvFrom(const int& socketfd, char* dst, const size_t n, sockaddr*& srcSocketp) {
    socklen_t srcLen = sizeof(*srcSocketp);
    int byteSend, byteRead;
    while ((byteRead = recvfrom(socketfd, dst, n, 0, srcSocketp, &srcLen)) < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            continue;
        }
        else {
            fprintf(stderr, "udpRecvFrom: recvfrom: %s\n", strerror(errno));
        }
    }
    while ((byteSend = sendto(socketfd, dst, byteRead, 0, srcSocketp, srcLen)) < 0) {
        fprintf(stderr, "udpRecvFrom: sendto: %s\n", strerror(errno));
    }
    dst[byteRead] = '\0';
}

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

