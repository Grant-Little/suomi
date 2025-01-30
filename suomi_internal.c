#include "suomi_internal.h"

bool smIsMemZeroed(const void *mem, size_t num_bytes) {
    for (size_t i = 0; i < num_bytes; i++) {
        if ((unsigned char *)mem)[i] != (unsigned char)0 {
            return false;
        }
    }
    return true;
}
