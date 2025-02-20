#ifndef SUOMI_H
#define SUOMI_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SM_NONE = 0,
    SM_ALLOCATION_FAILED,
    SM_ARENA_FULL,
    SM_BUFFER_TOO_SMALL,
    SM_INDEX_OUT_OF_BOUNDS,
    SM_REQUESTED_NULL,
} smError;

typedef struct {
    uintptr_t start_pos;
    uintptr_t end_pos;
    uintptr_t current_pos;
} smArena;

smArena smArenaInit(smError *error, size_t num_bytes);
void smArenaDeinit(smArena *arena);
void smArenaClear(smArena *arena);
void *smArenaPush(smError *error, smArena *arena, size_t num_bytes);
void smArenaPop(smArena *arena, size_t num_bytes);

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

smHashTable smHashTableInit(smError *error, smArena *arena, size_t value_num_bytes, size_t expected_num_values);
void smHashTableDeinit(smHashTable *hash_table);
void smHashTableClear(smHashTable *hash_table);

void smHashTableInsert(smError *error, smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value);
void *smHashTableRetrieve(const smHashTable *hash_table, const void *key, size_t key_num_bytes);
void smHashTableRemove(smHashTable *hash_table, const void *key, size_t key_num_bytes);
bool smHashTableIsFull(smHashTable *hash_table);

typedef struct {
    uintptr_t contents;
    size_t value_num_bytes;
    size_t num_values;
    uintptr_t front;
    uintptr_t end;
} smQueue;

smQueue smQueueInit(smError *error, smArena *arena, size_t value_num_bytes, size_t num_values);
void smQueueDeinit(smQueue *queue);
void smQueueClear(smQueue *queue);

void smQueueInsert(smError *error, smQueue *queue, void *value);
void *smQueueRetrieve(smError *error, smQueue *queue);
void *smQueuePeek(smError *error, smQueue *queue);
bool smQueueIsFull(smQueue *queue);

#endif
