#include "suomi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

bool smIsMemZeroed(const void *mem, size_t num_bytes);

smArena smArenaInit(smError *error, size_t num_bytes) {
    smArena arena = {
        .start_pos = 0,
        .end_pos = 0,
        .current_pos = 0,
    };

    arena.start_pos = (uintptr_t)malloc(num_bytes);
#ifndef SM_ASSURE
    if (!arena.start_pos) {
        *error = SM_ALLOCATION_FAILED;
        return arena;
    }
#endif
    arena.end_pos = arena.start_pos + num_bytes;
    arena.current_pos = arena.start_pos;

    return arena;
}

void smArenaDeinit(smArena *arena) {
    free((void *)(arena->start_pos));
    arena->start_pos = 0;
    arena->end_pos = 0;
    arena->current_pos = 0;
}

void smArenaClear(smArena *arena) {
    arena->current_pos = arena->start_pos;
}

void *smArenaPush(smError *error, smArena *arena, size_t num_bytes) {
#ifndef SM_ASSURE
    if ((arena->current_pos + num_bytes) > arena->end_pos) {
        *error = SM_ARENA_FULL;
        return NULL;
    }
#endif
    arena->current_pos += num_bytes;
    return (void *)(arena->current_pos - num_bytes);
}

void smArenaPop(smArena *arena, size_t num_bytes) {
    arena->current_pos -= num_bytes;
    if (arena->current_pos < arena->start_pos) {
        arena->current_pos = arena->start_pos;
    }
}

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

smHashTable smHashTableInit(smError *error, smArena *arena, size_t value_num_bytes, size_t expected_num_buckets) {
    smHashTable hash_table = {
        .buckets = 0,
        .bucket_num_bytes = 0,
        .max_num_buckets = 0,
        .num_used_buckets = 0,
    }; 

    size_t table_max_num_buckets = expected_num_buckets * 100u / SM_HASH_LOAD_FACTOR;
    size_t bytes_to_push = table_max_num_buckets * (value_num_bytes + sizeof(uint32_t));

    uintptr_t push_result = (uintptr_t)smArenaPush(error, arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!error) {
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

void smHashTableInsert(smError *error, smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value) {
#ifndef SM_ASSURE
    if (hash_table->num_used_buckets >= hash_table->max_num_buckets) {
        *error = SM_BUFFER_TOO_SMALL;
        return;
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
    return;
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

bool smHashTableIsFull(smHashTable *hash_table) {
    if (hash_table->num_used_buckets == hash_table->max_num_buckets) {
        return true;
    } else {
        return false;
    }
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

#define SM_QUEUE_IS_FULL(que) ((que->next_insert % que->num_values) == ((que->next_retrieve % que->num_values) - 1))
#define SM_QUEUE_IS_EMPTY(que) (que->next_insert == que->next_retrieve)

smQueue smQueueInit(smError *error, smArena *arena, size_t value_num_bytes, size_t num_values) {
    smQueue queue = {
        .contents = 0,
        .value_num_bytes = 0,
        .num_values = 0,
        .next_insert = 0,
        .next_retrieve = 0,
    };

    size_t bytes_to_push = value_num_bytes * num_values;
    uintptr_t push_result = (uintptr_t)smArenaPush(error, arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!error) {
        return queue;
    }
#endif
    queue.contents = push_result;
    queue.value_num_bytes = value_num_bytes;
    queue.num_values = num_values;

    return queue;
}

void smQueueDeinit(smQueue *queue) {
    queue->contents = 0;
    queue->value_num_bytes = 0;
    queue->num_values = 0;
    queue->next_insert = 0;
    queue->next_retrieve = 0;
}

void smQueueClear(smQueue *queue) {
    memset((void *)queue->contents, 0, queue->value_num_bytes * queue->num_values);
    queue->next_insert = 0;
    queue->next_retrieve = 0;
}

void smQueueInsert(smError *error, smQueue *queue, void *value) {
#ifndef SM_ASSURE
    if (SM_QUEUE_IS_FULL(queue)) {
        *error = SM_BUFFER_TOO_SMALL;
        return;
    }
#endif
    
    memcpy((void *)(queue->contents + (queue->next_insert % queue->num_values) * queue->value_num_bytes), value, queue->value_num_bytes);
    queue->next_insert += 1;
}

void *smQueueRetrieve(smError *error, smQueue *queue) {
#ifndef SM_ASSURE
    if (SM_QUEUE_IS_EMPTY(queue)) {
        *error = SM_REQUESTED_NULL;
        return NULL;
    }
#endif

    queue->next_retrieve += 1;
    return (void *)(queue->contents + ((queue->next_retrieve - 1) % queue->num_values) * queue->value_num_bytes);
}

void *smQueuePeek(smError *error, smQueue *queue) {
#ifndef SM_ASSURE
    if (SM_QUEUE_IS_EMPTY(queue)) {
        *error = SM_REQUESTED_NULL;
        return NULL;
    }
#endif

    return (void *)(queue->contents + (queue->next_retrieve % queue->num_values) * queue->value_num_bytes);
}

#define SM_HEAP_INDEX_LEFT(idx) (2 * idx + 1)
#define SM_HEAP_INDEX_RIGHT(idx) (2 * idx + 2)
#define SM_HEAP_INDEX_PARENT(idx) ((idx - 1) / 2)

smHeap smHeapInit(smError *error, smArena *arena, size_t value_num_bytes, size_t max_num_values, void (*comparison_function)(const void *, const void *, size_t), smHeapDirection direction) {
    smHeap heap = {
        .contents = 0,
        .value_num_bytes = 0,
        .current_num_values = 0,
        .max_num_values = 0,
        .comparison_function = NULL,
        .direction = 0,
    };

    size_t bytes_to_push = value_num_bytes * max_num_values;
    uintptr_t push_result = (uintptr_t)smArenaPush(error, arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!error) {
        return heap;
    }
#endif

    heap.contents = push_result;
    heap.value_num_bytes = value_num_bytes;
    heap.max_num_values = max_num_values;
    heap.comparison_function = comparison_function;
    heap.direction = direction;

    return heap;
}

void smHeapDeinit(smHeap *heap) {
    heap->contents = 0;
    heap->value_num_bytes = 0;
    heap->current_num_values = 0;
    heap->max_num_values = 0;
    heap->comparison_function = NULL;
    heap->direction = 0;
}

void smHeapClear(smHeap *heap) {
    memset((void *)heap->contents, 0, heap->value_num_bytes * heap->max_num_buckets);
    heap->current_num_values = 0;
}

bool smIsMemZeroed(const void *mem, size_t num_bytes) {
    for (size_t i = 0; i < num_bytes; i++) {
        if (((uint8_t *)mem)[i] != 0) {
            return false;
        }
    }
    return true;
}
