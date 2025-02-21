#include "suomi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

bool sm_is_mem_zeroed(const void *mem, size_t num_bytes);

sm_Arena sm_arena_init(sm_Error *error, size_t num_bytes) {
    sm_Arena arena = {
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

void sm_arena_deinit(sm_Arena *arena) {
    free((void *)(arena->start_pos));
    arena->start_pos = 0;
    arena->end_pos = 0;
    arena->current_pos = 0;
}

void sm_arena_clear(sm_Arena *arena) {
    arena->current_pos = arena->start_pos;
}

void *sm_arena_push(sm_Error *error, sm_Arena *arena, size_t num_bytes) {
#ifndef SM_ASSURE
    if ((arena->current_pos + num_bytes) > arena->end_pos) {
        *error = SM_ARENA_FULL;
        return NULL;
    }
#endif
    arena->current_pos += num_bytes;
    return (void *)(arena->current_pos - num_bytes);
}

void sm_arena_pop(sm_Arena *arena, size_t num_bytes) {
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

#define SM_HASH_FUNCTION sm_hash_fnv1a32

#define SM_HASH_TABLE_GET_BUCKET_HASH(bkt) (*((uint32_t *)bkt))

#define SM_HASH_TABLE_INDEX_TABLE(tbl, idx) (tbl->buckets + (index * tbl->bucket_num_bytes))

#define SM_HASH_TABLE_IS_OCCUPIED_INDEX(tbl, idx, hsh) (SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != 0 && SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != UINT32_MAX && SM_HASH_TABLE_GET_BUCKET_HASH(SM_HASH_TABLE_INDEX_TABLE(tbl, idx)) != hsh)

#define SM_HASH_TABLE_DELETE_BUCKET(tbl, idx) (*(uint32_t *)SM_HASH_TABLE_INDEX_TABLE(tbl, idx) = UINT32_MAX)

uint32_t sm_hash_internal_probe(const sm_Hash_Table *hash_table, uint32_t hash, uint32_t index);

uint32_t sm_hash_fnv1a32(const void *data, size_t data_num_bytes) {
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

uint32_t sm_hash_djb32(const void *data, size_t data_num_bytes) {
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

sm_Hash_Table sm_hash_table_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t expected_num_buckets) {
    sm_Hash_Table hash_table = {
        .buckets = 0,
        .bucket_num_bytes = 0,
        .max_num_buckets = 0,
        .num_used_buckets = 0,
    }; 

    size_t table_max_num_buckets = expected_num_buckets * 100u / SM_HASH_LOAD_FACTOR;
    size_t bytes_to_push = table_max_num_buckets * (value_num_bytes + sizeof(uint32_t));

    uintptr_t push_result = (uintptr_t)sm_arena_push(error, arena, bytes_to_push);
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

void sm_hash_table_deinit(sm_Hash_Table *hash_table) {
    hash_table->buckets = 0;
    hash_table->bucket_num_bytes = 0;
    hash_table->max_num_buckets = 0;
    hash_table->num_used_buckets = 0;
}

void sm_hash_table_clear(sm_Hash_Table *hash_table) {
    memset((void *)hash_table->buckets, 0, hash_table->max_num_buckets * hash_table->bucket_num_bytes);
    hash_table->num_used_buckets = 0;
}

void sm_hash_table_insert(sm_Error *error, sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes, const void *value) {
#ifndef SM_ASSURE
    if (hash_table->num_used_buckets >= hash_table->max_num_buckets) {
        *error = SM_BUFFER_TOO_SMALL;
        return;
    }
#endif
    uint32_t hash = SM_HASH_FUNCTION(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_buckets;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = sm_hash_internal_probe(hash_table, hash, index);
    }

    void *bucket_ptr = (void *)SM_HASH_TABLE_INDEX_TABLE(hash_table, index);
    *(uint32_t *)bucket_ptr = hash;
    memcpy((void *)(SM_HASH_TABLE_INDEX_TABLE(hash_table, index) + sizeof(uint32_t)), value, hash_table->bucket_num_bytes - sizeof(uint32_t));
    hash_table->num_used_buckets++;
    return;
}

void *sm_hash_table_retrieve(const sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes) {
    uint32_t hash = SM_HASH_FUNCTION(key, key_num_bytes);
    uint32_t index = hash % hash_table->max_num_buckets;

    if (SM_HASH_TABLE_IS_OCCUPIED_INDEX(hash_table, index, hash)) {
        index = sm_hash_internal_probe(hash_table, hash, index);
    }

    return (void *)(SM_HASH_TABLE_INDEX_TABLE(hash_table, index) + sizeof(uint32_t));
}

void sm_hash_table_remove(sm_Hash_Table *hash_table, const void *key, size_t key_num_bytes) {
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

bool sm_hash_table_is_full(const sm_Hash_Table *hash_table) {
    if (hash_table->num_used_buckets == hash_table->max_num_buckets) {
        return true;
    } else {
        return false;
    }
}

uint32_t sm_hash_internal_probe(const sm_Hash_Table *hash_table, uint32_t hash, uint32_t index) {
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

sm_Queue sm_queue_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t num_values) {
    sm_Queue queue = {
        .contents = 0,
        .value_num_bytes = 0,
        .num_values = 0,
        .next_insert = 0,
        .next_retrieve = 0,
    };

    size_t bytes_to_push = value_num_bytes * num_values;
    uintptr_t push_result = (uintptr_t)sm_arena_push(error, arena, bytes_to_push);
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

void sm_queue_deinit(sm_Queue *queue) {
    queue->contents = 0;
    queue->value_num_bytes = 0;
    queue->num_values = 0;
    queue->next_insert = 0;
    queue->next_retrieve = 0;
}

void sm_queue_clear(sm_Queue *queue) {
    memset((void *)queue->contents, 0, queue->value_num_bytes * queue->num_values);
    queue->next_insert = 0;
    queue->next_retrieve = 0;
}

void sm_queue_insert(sm_Error *error, sm_Queue *queue, void *value) {
#ifndef SM_ASSURE
    if (SM_QUEUE_IS_FULL(queue)) {
        *error = SM_BUFFER_TOO_SMALL;
        return;
    }
#endif
    
    memcpy((void *)(queue->contents + (queue->next_insert % queue->num_values) * queue->value_num_bytes), value, queue->value_num_bytes);
    queue->next_insert += 1;
}

void *sm_queue_retrieve(sm_Error *error, sm_Queue *queue) {
#ifndef SM_ASSURE
    if (SM_QUEUE_IS_EMPTY(queue)) {
        *error = SM_REQUESTED_NULL;
        return NULL;
    }
#endif

    queue->next_retrieve++;
    return (void *)(queue->contents + ((queue->next_retrieve - 1) % queue->num_values) * queue->value_num_bytes);
}

void *sm_queue_peek(sm_Error *error, const sm_Queue *queue) {
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

sm_Heap sm_heap_init(sm_Error *error, sm_Arena *arena, size_t value_num_bytes, size_t max_num_values, void (*comparison_function)(const void *, const void *, size_t), sm_Heap_Type max_or_min) {
    sm_Heap heap = {
        .contents = 0,
        .value_num_bytes = 0,
        .current_num_values = 0,
        .max_num_values = 0,
        .comparison_function = NULL,
        .type = 0,
    };

    size_t bytes_to_push = value_num_bytes * max_num_values;
    uintptr_t push_result = (uintptr_t)sm_arena_push(error, arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!error) {
        return heap;
    }
#endif

    heap.contents = push_result;
    heap.value_num_bytes = value_num_bytes;
    heap.max_num_values = max_num_values;
    heap.comparison_function = comparison_function;
    heap.type = max_or_min;

    return heap;
}

void sm_heap_deinit(sm_Heap *heap) {
    heap->contents = 0;
    heap->value_num_bytes = 0;
    heap->current_num_values = 0;
    heap->max_num_values = 0;
    heap->comparison_function = NULL;
    heap->type = 0;
}

void sm_heap_clear(sm_Heap *heap) {
    memset((void *)heap->contents, 0, heap->value_num_bytes * heap->max_num_values);
    heap->current_num_values = 0;
}

bool sm_is_mem_zeroed(const void *mem, size_t num_bytes) {
    for (size_t i = 0; i < num_bytes; i++) {
        if (((uint8_t *)mem)[i] != 0) {
            return false;
        }
    }
    return true;
}
