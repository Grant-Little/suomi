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
} sm_Error;

typedef struct {
    uintptr_t start_pos;
    uintptr_t end_pos;
    uintptr_t current_pos;
} sm_Arena;

sm_Arena sm_arena_init(sm_Error *error, size_t num_bytes);
void sm_arena_deinit(sm_Arena *arena);
void sm_arena_clear(sm_Arena *arena);
void *sm_arena_push(sm_Error *error, sm_Arena *arena, size_t num_bytes);
void sm_arena_pop(sm_Arena *arena, size_t num_bytes);

typedef struct {
    uintptr_t buckets;
    size_t bucket_num_bytes;
    uint32_t max_num_buckets;
    uint32_t num_used_buckets;
} sm_Hash_Table;

// default hash function
uint32_t sm_hash_fnv1a32(const void *data, size_t data_num_bytes);

// extra hash functions
uint32_t sm_hash_djb32(const void *data, size_t data_num_bytes);

sm_Hash_Table sm_hash_table_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t expected_num_values);
void sm_hash_table_deinit(sm_Hash_Table *hash_table);
void sm_hash_table_clear(sm_Hash_Table *hash_table);

void sm_hash_table_insert(sm_Error *error, sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes, const void *value);
void *sm_hash_table_retrieve(const sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes);
void sm_hash_table_remove(sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes);
bool sm_hash_table_is_full(const sm_Hash_Table *hash_table);

typedef struct {
    uintptr_t contents;
    size_t value_num_bytes;
    size_t num_values;
    ptrdiff_t next_insert;
    ptrdiff_t next_retrieve;
} sm_Queue;

sm_Queue sm_queue_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t num_values);
void sm_queue_deinit(sm_Queue *queue);
void sm_queue_clear(sm_Queue *queue);

void sm_queue_insert(sm_Error *error, sm_Queue *queue, void *value);
void *sm_queue_retrieve(sm_Error *error, sm_Queue *queue);
void *sm_queue_peek(sm_Error *error, const sm_Queue *queue);
bool sm_queue_is_full(const sm_Queue *queue);

typedef enum {
    SM_HEAP_MAX,
    SM_HEAP_MIN,
} sm_Heap_Type;

typedef struct {
    uintptr_t contents;
    size_t value_num_bytes;
    size_t current_num_values;
    size_t max_num_values;
    void (*comparison_function)(const void *, const void *, size_t);
    sm_Heap_Type type;
} sm_Heap;

sm_Heap sm_heap_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t max_num_values, void (*comparison_function)(const void *, const void *, size_t), sm_Heap_Type max_or_min);
sm_Heap sm_heap_heapify(sm_Error *error, sm_Arena *arena, void *values, size_t num_values, size_t value_num_bytes, size_t max_num_values, void (*comparison_function)(const void *, const void *, size_t), sm_Heap_Type max_or_min); // creates a heap inplace on a given array, (can this even error?)
void sm_heap_deinit(sm_Heap *heap);
void sm_heap_clear(sm_Heap *heap);

void sm_heap_insert(sm_Error *error, sm_Heap *heap, void *value);
void *sm_heap_retrieve(sm_Error *error, sm_Heap *heap);
void *sm_heap_peek(sm_Error *error, const sm_Heap *heap);
bool sm_heap_is_full(sm_Error *error, const sm_Heap *heap);

#endif
