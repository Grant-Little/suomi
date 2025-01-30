#ifndef SUOMI_H
#define SUOMI_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uintptr_t start_pos;
    uintptr_t end_pos;
    uintptr_t current_pos;
} smArena;

smArena smArenaInit(size_t num_bytes);
void smArenaDeinit(smArena *arena);
void smArenaClear(smArena *arena);
void *smArenaPush(smArena *arena, size_t num_bytes);
void smArenaPop(smArena *arena, size_t num_bytes);

typedef struct {
    char *contents;
    size_t length;
    size_t capacity;
} smString;

// displaying strings
void smStringPuts(const smString *string);

// creating and destroying strings
smString smStringInit(smArena *arena, size_t capacity);
smString smStringInitWithContents(smArena *arena, const char *contents, size_t capacity);
void smStringDeinit(smString *string);
void smStringClear(smString *string);

// comparing string contents
// if you want to directly compare string structs, then do:
// memcmp(&string1, &string2, sizeof(smString))
bool smStringAreContentsSame(const smString *string1, const smString *string2);

// index string
char smStringIndex(const smString *string, size_t index); //positive indices, return '\0' on invalid index

// modifying strings
int smStringAppendCstring(smString *dest_string, const char *cstring);
int smStringAppendString(smString *dest_string, const smString *source_string);
int smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
int smStringCopyCstring(smString *dest_string , const char *cstring);
int smStringCopyString(smString *dest_string, const smString *copied_string);
int smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
int smStringWriteCstringAtIndex(smString *dest_string, size_t dest_index, const char *cstring);
int smStringWriteStringAtIndex(smString *dest_string, size_t dest_index, const smString *source_string);
int smStringWriteSubStringAtIndex(smString *dest_string, size_t dest_index, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
int smStringRemove(smString *string, size_t index, size_t length);
int smStringPush(smString *string, char character);
char smStringPop(smString *string);

// search for first occurence, provide index and length to search, return SIZE_MAX if not found, 0 search_length is taken as searched_string->length
// uses memmem which is not widely supported
// potentially write own implementation of memmem to use if memmem not defined
size_t smStringFindCstring(const smString *searched_string, size_t search_index, size_t search_length, const char *cstring);
size_t smStringFindString(const smString *searched_string, size_t search_index, size_t search_length, const smString *find_string);
size_t smStringFindSubString(const smString *searched_string, size_t search_index, size_t search_length, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);

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
