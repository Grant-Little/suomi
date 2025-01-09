#include "suomi_hash_table.h"
#include "string.h"

#define SM_HASH_FNV_PRIME_32 16777619u
#define SM_HASH_FNV_OFFSET_32 2166136261u
#define SM_HASH_LOAD_FACTOR 70u

uint32_t smFnv1aHash32(const void *data, size_t data_num_bytes) {
    uint32_t hash = SM_HASH_FNV_OFFSET_32;

    for (size_t i = 0; i < data_num_bytes; i++) {
        hash ^= ((uint8_t *)(data))[i];
        hash *= SM_HASH_FNV_PRIME_32;
    }

    return hash;
}

smHashTable smHashTableInit(smArena *arena, size_t value_num_bytes, size_t expected_num_values) {
    smHashTable hash_table = {
        .hashes = NULL,
        .values = 0,
        .value_num_bytes = 0,
        .max_num_entries = 0,
        .num_current_entries = 0,
        .hash_function = NULL,
    };

    size_t table_max_num_entries = expected_num_values * 100u / SM_HASH_LOAD_FACTOR;
    size_t bytes_to_push = table_max_num_entries * (value_num_bytes + 4);

    uintptr_t push_result = (uintptr_t)smArenaPush(arena, bytes_to_push);
    if (!push_result) {
        return hash_table;
    } else {
        hash_table.hashes = (uint32_t *)push_result;
        hash_table.values = (table_max_num_entries * 4) + push_result;
        hash_table.value_num_bytes = value_num_bytes;
        hash_table.max_num_entries = table_max_num_entries;
        hash_table.hash_function = smFnv1aHash32;
        return hash_table;
    }
}

void smHashTableDeinit(smHashTable *hash_table) {
    hash_table->hashes = NULL;
    hash_table->values = 0;
    hash_table->value_num_bytes = 0;
    hash_table->max_num_entries = 0;
    hash_table->num_current_entries = 0;
    hash_table->hash_function = NULL;
}

void smHashTableClear(smHashTable *hash_table) {
    memset(hash_table->hashes, 0, hash_table->max_num_entries * 4);
    hash_table->num_current_entries = 0;
}
