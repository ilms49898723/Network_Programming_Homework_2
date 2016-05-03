#ifndef NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H
#define NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

#include <cstdlib>
#include "global.h"

void setSocketTimeout(const int& socketfd, const int& second, const int& millisecond);

int stringHash(const char* src, size_t len);

#endif // NETWORK_PROGRAMMING_HOMEWORK_2_NPUTILITY_H

