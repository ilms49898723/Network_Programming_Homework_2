#include "UDPUtil.h"


UDPSeqCounter::UDPSeqCounter() {
    init();
}

UDPSeqCounter::~UDPSeqCounter() {

}

void UDPSeqCounter::init() {
    seq = 0;
}

void UDPSeqCounter::incSeq() {
    seq++;
}

void UDPSeqCounter::setSeq(const unsigned long long& seq) {
    this->seq = seq;
}

unsigned long long UDPSeqCounter::getSeq() const {
    return seq;
}


UDPUtil::UDPUtil() {
    cnt.init();
    lastSeq = 0;
}

UDPUtil::~UDPUtil() {

}

int UDPUtil::udpTrans(int fd, sockaddr*& sockp, char* dst, size_t dn, const char* src, size_t sn) {
    socklen_t sockLen = sizeof(*sockp);
    unsigned long long thisSeq = cnt.getSeq();
    cnt.incSeq();
    char toSend[PMAXN];
    char toRecv[PMAXN];
    memset(toSend, 0, sizeof(toSend));
    memcpy(toSend, src, sn);
    pack(toSend, thisSeq);
    int byteSend;
    int counter;
    counter = 0;
    while ((byteSend = sendto(fd, toSend, sn + 20, 0, sockp, sockLen)) < 0) {
        counter++;
        if (counter > 5) {
            return -1;
        }
        fprintf(stderr, "UDPUtil: sendto: %s\n",strerror(errno));
    }
    int byteRecv;
    counter = 0;
    while (true) {
        memset(toRecv, 0, sizeof(toRecv));
        byteRecv = recvfrom(fd, toRecv, PMAXN, 0, sockp, &sockLen);
        if (byteRecv < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                if ((byteSend = sendto(fd, toSend, sn + 20, 0, sockp, sockLen)) < 0) {
                    fprintf(stderr, "UDPUtil: sendto: %s\n", strerror(errno));
                    return -1;
                }
            }
            else {
                fprintf(stderr, "UDPUtil: recvfrom: %s\n", strerror(errno));
                return -1;
            }
        }
        else if (byteRecv < 20) {
            continue;
        }
        else {
            unsigned long long seq = getSeq(toRecv);
            if (seq == thisSeq) {
                unpack(toRecv);
                memset(dst, 0, sizeof(char) * dn);
                memcpy(dst, toRecv, dn);
                dst[byteRecv - 20] = '\0';
                return byteRecv - 20;
            }
        }
    }
}

int UDPUtil::udpSend(int fd, sockaddr*& sockp, const char* src, size_t n) {
    socklen_t sockLen = sizeof(*sockp);
    char temp[PMAXN];
    memset(temp, 0, sizeof(temp));
    memcpy(temp, src, n);
    pack(temp, lastSeq);
    int byteSend = sendto(fd, temp, n + 20, 0, sockp, sockLen);
    if (byteSend < 0) {
        fprintf(stderr, "UDPUtil: udpSend: %s\n", strerror(errno));
    }
    return byteSend;
}


int UDPUtil::udpRecv(int fd, sockaddr*& sockp, char* dst, size_t n) {
    socklen_t sockLen = sizeof(*sockp);
    char temp[PMAXN];
    int byteRecv = recvfrom(fd, temp, PMAXN, 0, sockp, &sockLen);
    if (byteRecv < 20) {
        fprintf(stderr, "UDPUtil: udpRecv: %s\n", strerror(errno));
        return -1;
    }
    lastSeq = getSeq(temp);
    unpack(temp);
    memcpy(dst, temp, n);
    dst[byteRecv - 20] = '\0';
    return byteRecv - 20;
}

unsigned long long UDPUtil::getSeq(const char* src) {
    char tmpStr[20];
    memset(tmpStr, 0, sizeof(tmpStr));
    memcpy(tmpStr, src + 4, 16);
    unsigned long long ret;
    sscanf(tmpStr, "%llx", &ret);
    return ret;
}

void UDPUtil::pack(char* src, unsigned long long seq) {
    char prefix[] = "\x02\x02";
    char postfix[] = "\x03\x03";
    char tmp[PMAXN];
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, prefix, 2);
    snprintf(tmp + 2, PMAXN - 2, "%016llx", seq);
    memcpy(tmp + 18, postfix, 2);
    memcpy(tmp + 20, src, PMAXN - 20);
    memset(src, 0, sizeof(char) * PMAXN);
    memcpy(src, tmp, PMAXN);
}

void UDPUtil::unpack(char* src) {
    char tmp[PMAXN];
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, src + 20, PMAXN - 20);
    memset(src, 0, sizeof(char) * PMAXN);
    memcpy(src, tmp, PMAXN);
}
