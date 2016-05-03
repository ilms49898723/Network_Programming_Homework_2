#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H
#define NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "global.h"

void udpSendTo(const int& socketfd, const char* msg, const size_t n, sockaddr*& dstSocketp);

void udpRecvFrom(const int& socketfd, char* dst, const size_t n, sockaddr*& srcSocketp);

void setSocketTimeout(const int& socketfd, const int& second, const int& millisecond);

void trimNewLine(char* src);

int stringHash(const char* src, size_t len);

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

