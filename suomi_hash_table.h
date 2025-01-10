#include "suomi_arena.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t *hashes;
    uintptr_t values;
    size_t value_num_bytes;
    uint32_t max_num_entries;
    uint32_t num_current_entries;
    uint32_t (*hash_function)(const void *, size_t);
} smHashTable;

// default hash function
uint32_t smHashFnv1a32(const void *data, size_t data_num_bytes);

smHashTable smHashTableInit(smArena *arena, size_t value_num_bytes, size_t expected_num_values);
void smHashTableDeinit(smHashTable *hash_table);
void smHashTableClear(smHashTable *hash_table);

bool smHashTableSet(smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value);
void *smHashTableRetrieve(const smHashTable *hash_table, const void *key, size_t key_num_bytes);
void smHashTableRemove(smHashTable *hash_table, const void *key, size_t key_num_bytes);
