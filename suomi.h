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
    char *contents;
    size_t length;
    size_t capacity;
} smString;

// displaying strings
void smStringPuts(const smString *string);

// creating and destroying strings
smString smStringInit(smError *error, smArena *arena, size_t capacity);
smString smStringInitWithContents(smError *error, smArena *arena, const char *contents, size_t capacity);
void smStringDeinit(smString *string);
void smStringClear(smString *string);

// comparing string contents
// if you want to directly compare string structs, then do:
// memcmp(&string1, &string2, sizeof(smString))
bool smStringAreContentsSame(const smString *string1, const smString *string2);

// index string
char smStringIndex(smError *error, const smString *string, size_t index); //positive indices, return '\0' on invalid index

// modifying strings
void smStringAppendCstring(smError *error, smString *dest_string, const char *cstring);
void smStringAppendString(smError *error, smString *dest_string, const smString *source_string);
void smStringAppendSubString(smError *error, smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
void smStringCopyCstring(smError *error, smString *dest_string , const char *cstring);
void smStringCopyString(smError *error, smString *dest_string, const smString *copied_string);
void smStringCopySubString(smError *error, smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
void smStringWriteCstringAtIndex(smError *error, smString *dest_string, size_t dest_index, const char *cstring);
void smStringWriteStringAtIndex(smError *error, smString *dest_string, size_t dest_index, const smString *source_string);
void smStringWriteSubStringAtIndex(smError *error, smString *dest_string, size_t dest_index, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
void smStringRemove(smError *error, smString *string, size_t index, size_t length);
void smStringPush(smError *error, smString *string, char character);
char smStringPop(smString *string);

// search for first occurence, provide index and length to search, return SIZE_MAX if not found, 0 search_length is taken as searched_string->length
// uses memmem which is not widely supported
// potentially write own implementation of memmem to use if memmem not defined
size_t smStringFindCstring(const smString *searched_string, size_t search_index, size_t search_length, const char *cstring);
size_t smStringFindString(const smString *searched_string, size_t search_index, size_t search_length, const smString *find_string);
size_t smStringFindSubString(smError *error, const smString *searched_string, size_t search_index, size_t search_length, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);

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
    void *value;
    void *next_node;
    void *previous_node;
} smLinkedListNode;

smLinkedListNode *smLinkedListInsert(smError *error, smArena *arena, smLinkedListNode *previous_node, void *value);
void smLinkedListRemove(smLinkedListNode *node_to_remove);
smLinkedListNode *smLinkedListNodeTraverse(smError *error, smLinkedListNode *start_node, int traverse_steps);

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
