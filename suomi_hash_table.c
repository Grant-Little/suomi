#include "suomi_hash_table.h"
#include <string.h>
#include <stdlib.h>

#define SM_HASH_LOAD_FACTOR 70u

#define SM_HASH_FNV_PRIME_32 16777619u
#define SM_HASH_FNV_OFFSET_32 2166136261u

#define SM_HASH_DJB_OFFSET_32 5381u
#define SM_HASH_DJB_SHIFT_32 5u

#define SM_HASH_FUNCTION smHashFnv1a32

#define SM_HASH_TABLE_GET_BUCKET_HASH(bkt) (*((uint32_t *)bkt))

#define SM_HASH_TABLE_INDEX_TABLE(tbl, idx) (tbl->buckets + (index * tbl->bucket_num_bytes))

#define SM_HASH_TABLE_IS_OCCUPIED_INDEX(tbl, idx, hsh) (SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != 0 && SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != UINT32_MAX && SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != hsh)

#define SM_HASH_TABLE_DELETE_BUCKET(tbl, idx) (*(uint32_t *)SM_HASH_TABLE_INDEX_TABLE(tbl, idx) = UINT32_MAX)

uint32_t smHashInternalProbe(const smHashTable *hash_table, uint32_t hash, uint32_t index);

uint32_t smHashFnv1a32(const void *data, size_t data_num_bytes) {
    uint32_t hash = SM_HASH_FNV_OFFSET_32;

    for (size_t i = 0; i < data_num_bytes; i++) {
        hash ^= ((uint8_t *)(data))[i];
        hash *= SM_HASH_FNV_PRIME_32;
    }

#ifndef SM_ASSURE
    if (hash == 0) {
        hash = 1;
    } else if (hash == UINT32_MAX) {
        hash = UINT32_MAX - 1;
    }
#endif

    return hash;
}

uint32_t smHashDjb32(const void *data, size_t data_num_bytes) {
    uint32_t hash = SM_HASH_DJB_OFFSET_32;

    for (size_t i = 0; i < data_num_bytes; i++) {
        hash = ((hash << SM_HASH_DJB_SHIFT_32) + hash) + ((uint8_t *)(data))[i];
    }

#ifndef SM_ASSURE
    if (hash == 0) {
        hash = 1;
    } else if (hash == UINT32_MAX) {
        hash = UINT32_MAX - 1;
    }
#endif

    return hash;
}

smHashTable smHashTableInit(smArena *arena, size_t value_num_bytes, size_t expected_num_buckets) {
    smHashTable hash_table = {
        .buckets = 0,
        .bucket_num_bytes = 0,
        .max_num_buckets = 0,
        .num_used_buckets = 0,
    }; 

    size_t table_max_num_buckets = expected_num_buckets * 100u / SM_HASH_LOAD_FACTOR;
    size_t bytes_to_push = table_max_num_buckets * (value_num_bytes + sizeof(uint32_t));

    uintptr_t push_result = (uintptr_t)smArenaPush(arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!push_result) {
        return hash_table;
    }
#endif
    memset((void *)push_result, 0, table_max_num_buckets * (value_num_bytes + sizeof(uint32_t)));
    hash_table.buckets = push_result;
    hash_table.bucket_num_bytes = value_num_bytes + sizeof(uint32_t);
    hash_table.max_num_buckets = table_max_num_buckets;
    return hash_table;
}

void smHashTableDeinit(smHashTable *hash_table) {
    hash_table->buckets = 0;
    hash_table->bucket_num_bytes = 0;
    hash_table->max_num_buckets = 0;
    hash_table->num_used_buckets = 0;
}

void smHashTableClear(smHashTable *hash_table) {
    memset((void *)hash_table->buckets, 0, hash_table->max_num_buckets * hash_table->bucket_num_bytes);
    hash_table->num_used_buckets = 0;
}

int smHashTableSet(smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value) {
#ifndef SM_ASSURE
    if (hash_table->num_used_buckets >= hash_table->max_num_buckets) {
        return EXIT_FAILURE;
    }
#endif
    uint32_t hash = SM_HASH_FUNCTION(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_buckets;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = smHashInternalProbe(hash_table, hash, index);
    }

    void *bucket_ptr = (void *)SM_HASH_TABLE_INDEX_TABLE(hash_table, index);
    *(uint32_t *)bucket_ptr = hash;
    memcpy((void *)(SM_HASH_TABLE_INDEX_TABLE(hash_table, index) + sizeof(uint32_t)), value, hash_table->bucket_num_bytes - sizeof(uint32_t));
    hash_table->num_used_buckets++;
    return EXIT_SUCCESS;
}

void *smHashTableRetrieve(const smHashTable *hash_table, const void *key, size_t key_num_bytes) {
    uint32_t hash = SM_HASH_FUNCTION(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_buckets;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = smHashInternalProbe(hash_table, hash, index);
    }

    return (void *)(SM_HASH_TABLE_INDEX_TABLE(hash_table, index) + sizeof(uint32_t));
}

void smHashTableRemove(smHashTable *hash_table, const void *key, size_t key_num_bytes) {
#ifndef SM_ASSURE
    if (hash_table->num_used_buckets == 0) {
        return;
    }
#endif
    uint32_t hash = SM_HASH_FUNCTION(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_buckets;

    uint32_t internal_probe = hash % (hash_table->max_num_buckets - 1);

    while (1) {
        if (SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(hash_table, index)) == hash) {
            *(uint32_t *)SM_HASH_TABLE_INDEX_TABLE(hash_table, index) = UINT32_MAX;
            SM_HASH_TABLE_DELETE_BUCKET(hash_table, index);
            hash_table->num_used_buckets--;
            break;
        } else if (SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(hash_table, index)) == 0) {
            break;
        } else {
            index += internal_probe;
            if (index >= hash_table->max_num_buckets) {
                index -= hash_table->max_num_buckets;
            }
        }
    }
    return;
}

uint32_t smHashInternalProbe(const smHashTable *hash_table, uint32_t hash, uint32_t index) {
    uint32_t internal_probe = hash % (hash_table->max_num_buckets - 1);
    while (1) {
        index += internal_probe;
        if (index >= hash_table->max_num_buckets) {
            index -= hash_table->max_num_buckets;
        }
        if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
            internal_probe++;
        }
        else {
            break;
        }
    }
    return index;
}
