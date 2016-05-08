#include "nputility.h"

void setSocketTimeout(const int& socketfd, const int& second, const int& millisecond) {
    timeval tv;
    tv.tv_sec = second;
    tv.tv_usec = millisecond * 1000;
    if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "setsockopt: %s\n", strerror(errno));
    }
    tv.tv_sec = second;
    tv.tv_usec = millisecond * 1000;
    if (setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "setsockopt: %s\n", strerror(errno));
    }
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

unsigned long long fileHash(const std::string& filename) {
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) < 0) {
        return 0xFFFFFFFFFFFFFFFFLL;
    }
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        return 0xFFFFFFFFFFFFFFFFLL;
    }
    unsigned c = 0xdefaced;
    unsigned long long cl = 0xdefaceddefacedLL;
    unsigned coef = 31;
    unsigned char buffer[4];
    unsigned long long ret = 0u;
    ret = static_cast<unsigned long long>(c) * c * coef * fileStat.st_size * (cl ^ fileStat.st_size);
    for (int i = 0; i < 1024; ++i) {
        memset(buffer, 0, sizeof(buffer));
        int n = read(fileno(fp), buffer, sizeof(unsigned char) * 4);
        if (n < 4) {
            break;
        }
        unsigned tmp = (buffer[0] << 24) |
                       (buffer[1] << 16) |
                       (buffer[2] << 8) |
                       (buffer[3]);
        ret += (tmp & c) * coef + c;
        coef *= 31;
    }
    fclose(fp);
    return ret;
}

ConnectInfo getConnectInfo(const sockaddr_in& sock) {
    ConnectInfo ret;
    ret.address = inet_ntoa(sock.sin_addr);
    ret.port = ntohs(sock.sin_port);
    return ret;
}

