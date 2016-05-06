#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H
#define NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <string>
#include "global.h"

struct ConnectInfo {
    std::string address;
    int port;
    ConnectInfo(const std::string& address = "", const int& port = 0) :
        address(address), port(port) { }
};

void setSocketTimeout(const int& socketfd, const int& second, const int& millisecond);

void trimNewLine(char* src);

int stringHash(const char* src, size_t len);

unsigned long long fileHash(const std::string& filename);

ConnectInfo getConnectInfo(const sockaddr_in& sock);

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

