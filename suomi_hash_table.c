#include "suomi_hash_table.h"
#include <string.h>
#include <stdlib.h>

#define SM_HASH_LOAD_FACTOR 70u

#define SM_HASH_FNV_PRIME_32 16777619u
#define SM_HASH_FNV_OFFSET_32 2166136261u

#define SM_HASH_DJB_OFFSET_32 5381u
#define SM_HASH_DJB_SHIFT_32 5u

#define SM_HASH_TABLE_IS_OCCUPIED_INDEX(tbl, idx, hsh) (tbl->hashes[idx] != 0 && tbl->hashes[idx] != UINT32_MAX && tbl->hashes[idx] != hsh)

uint32_t smHashInternalProbe(const smHashTable *hash_table, uint32_t hash, uint32_t index);

uint32_t smHashFnv1a32(const void *data, size_t data_num_bytes) {
    uint32_t hash = SM_HASH_FNV_OFFSET_32;

    for (size_t i = 0; i < data_num_bytes; i++) {
        hash ^= ((uint8_t *)(data))[i];
        hash *= SM_HASH_FNV_PRIME_32;
    }

    if (hash == 0) {
        hash = 1;
    } else if (hash == UINT32_MAX) {
        hash = UINT32_MAX - 1;
    }

    return hash;
}

uint32_t smHashDjb32(const void *data, size_t data_num_bytes) {
    uint32_t hash = SM_HASH_DJB_OFFSET_32;

    for (size_t i = 0; i < data_num_bytes; i++) {
        hash = ((hash << SM_HASH_DJB_SHIFT_32) + hash) + ((uint8_t *)(data))[i];
    }

    if (hash == 0) {
        hash = 1;
    } else if (hash == UINT32_MAX) {
        hash = UINT32_MAX - 1;
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
        memset((void *)push_result, 0, table_max_num_entries * 4);
        hash_table.hashes = (uint32_t *)push_result;
        hash_table.values = (table_max_num_entries * 4) + push_result;
        hash_table.value_num_bytes = value_num_bytes;
        hash_table.max_num_entries = table_max_num_entries;
        hash_table.hash_function = smHashFnv1a32;
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

int smHashTableSet(smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value) {
    if (hash_table->num_current_entries >= hash_table->max_num_entries) {
        return EXIT_FAILURE;
    }

    uint32_t hash = hash_table->hash_function(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_entries;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = smHashInternalProbe(hash_table, hash, index);
    }

    hash_table->hashes[index] = hash;
    memcpy((void *)(hash_table->values + index * hash_table->value_num_bytes), value, hash_table->value_num_bytes);
    hash_table->num_current_entries++;
    return EXIT_SUCCESS;
}

void *smHashTableRetrieve(const smHashTable *hash_table, const void *key, size_t key_num_bytes) {
    uint32_t hash = hash_table->hash_function(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_entries;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = smHashInternalProbe(hash_table, hash, index);
    }

    return (void *)(hash_table->values + index * hash_table->value_num_bytes);
}

void smHashTableRemove(smHashTable *hash_table, const void *key, size_t key_num_bytes) {
    if (hash_table->num_current_entries == 0) {
        return;
    }

    uint32_t hash = hash_table->hash_function(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_entries;

    uint32_t internal_probe = hash % (hash_table->max_num_entries - 1);

    while (1) {
        if (hash_table->hashes[index] == hash) {
            hash_table->hashes[index] = UINT32_MAX;
            hash_table->num_current_entries--;
            break;
        } else if (hash_table->hashes[index] == 0) {
            break;
        } else {
            index += internal_probe;
            if (index >= hash_table->max_num_entries) {
                index -= hash_table->max_num_entries;
            }
            internal_probe++;
        }
    }
}

uint32_t smHashInternalProbe(const smHashTable *hash_table, uint32_t hash, uint32_t index) {
    uint32_t internal_probe = hash % (hash_table->max_num_entries - 1);
    while (1) {
        index += internal_probe;
        if (index >= hash_table->max_num_entries) {
            index -= hash_table->max_num_entries;
        }
        if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
            internal_probe++;
        } else {
            break;
        }
    }
    return index;
}
