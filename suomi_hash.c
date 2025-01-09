#include "suomi_hash.h"

#define SM_HASH_FNV_PRIME_32 16777619u
#define SM_HASH_FNV_OFFSET_32 2166136261u
#define SM_HASH_LOAD_FACTOR 0.7f

uint32_t smFnv1aHash32(const void *data, size_t data_length) {
    uint32_t hash = SM_HASH_FNV_OFFSET_32;

    for (size_t i = 0; i < data_length; i++) {
        hash ^= ((uint8_t *)(data))[i];
        hash *= SM_HASH_FNV_PRIME_32;
    }

    return hash;
}
