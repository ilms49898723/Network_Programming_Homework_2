#include "nputility.h"

int stringHash(const char* src, size_t len) {
    const int coef = 31;
    int ret = 0;
    for (size_t i = 0; i < len; ++i) {
        ret = ret * coef + src[i];
        corf *= 31;
    }
    return ret;
}

