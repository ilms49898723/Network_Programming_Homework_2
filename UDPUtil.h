#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_UDPUTIL_H_
#define NETWORK_PROGRAMMING_HOMEWORK_2_UDPUTIL_H_

#include <cstdio>
#include <cstring>
#include <memory.h>
#include "npinc.h"
#include "nputility.h"

class UDPSeqCounter {
    public:
        UDPSeqCounter();

        virtual ~UDPSeqCounter();

        void init();

        void incSeq();

        void setSeq(const unsigned long long& seq);

        unsigned long long getSeq() const;

    private:
        unsigned long long seq;
};

class UDPUtil {
    public:
        constexpr static int PMAXN = 2200;

    public:
        unsigned long long lastSeq;

    public:
        UDPUtil();

        virtual ~UDPUtil();

        int udpTrans(int fd, sockaddr*& sockp, char* dst, size_t dn, const char* src, size_t sn);

        int udpSend(int fd, sockaddr*& sockp, const char* src, size_t n);

        int udpRecv(int fd, sockaddr*& sockp, char* dst, size_t n);

    private:
        unsigned long long getSeq(const char* src);

        void pack(char* src, unsigned long long seq);

        void unpack(char* src);

    private:
        UDPSeqCounter cnt;
};

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_UDPUTIL_H_

