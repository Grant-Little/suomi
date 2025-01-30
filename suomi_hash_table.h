#ifndef SUOMI_HASH_TABLE_H
#define SUOMI_HASH_TABLE_H

#include "suomi_arena.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uintptr_t buckets;
    size_t bucket_num_bytes;
    uint32_t max_num_buckets;
    uint32_t num_used_buckets;
} smHashTable;

// default hash function
uint32_t smHashFnv1a32(const void *data, size_t data_num_bytes);

// extra hash functions
uint32_t smHashDjb32(const void *data, size_t data_num_bytes);

smHashTable smHashTableInit(smArena *arena, size_t value_num_bytes, size_t expected_num_values);
void smHashTableDeinit(smHashTable *hash_table);
void smHashTableClear(smHashTable *hash_table);

int smHashTableSet(smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value);
void *smHashTableRetrieve(const smHashTable *hash_table, const void *key, size_t key_num_bytes);
void smHashTableRemove(smHashTable *hash_table, const void *key, size_t key_num_bytes);

#endif
