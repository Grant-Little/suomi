#include "suomi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

bool smIsMemZeroed(const void *mem, size_t num_bytes);
void *smMemMem(const void *haystack, size_t haystack_length, const void *needle, size_t needle_length);

smArena smArenaInit(size_t num_bytes) {
    smArena arena = {
        .start_pos = 0,
        .end_pos = 0,
        .current_pos = 0,
    };

    arena.start_pos = (uintptr_t)malloc(num_bytes);
#ifndef SM_ASSURE
    if (!arena.start_pos) {
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

void *smArenaPush(smArena *arena, size_t num_bytes) {
#ifndef SM_ASSURE
    if ((arena->current_pos + num_bytes) > arena->end_pos) {
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

#define SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length) (sub_string_index < sub_string->length && (sub_string_index + sub_string_length) <= sub_string->length)

void smStringPuts(const smString *string) {
    for (size_t i = 0; i < string->length; i++) {
        putchar(string->contents[i]);
    }
    putchar('\n');
}

smString smStringInit(smArena *arena, size_t capacity) {
    smString string = {
        .contents = NULL,
        .length = 0,
        .capacity = 0,
    };

    char *push_result = smArenaPush(arena, capacity);
#ifndef SM_ASSURE
    if (!push_result) {
        return string;
    }
#endif
    string.contents = push_result;
    string.capacity = capacity;
    return string;
}

smString smStringInitWithContents(smArena *arena, const char *contents, size_t capacity) {
    smString string = {
        .contents = NULL,
        .length = 0,
        .capacity = 0,
    };
    
    size_t contents_length = strlen(contents);
    // we don't do an SM_ASSURE thing here
    // because it would completely break functionality
    if (capacity < contents_length) {
        capacity = contents_length;
    } else if (contents_length > capacity) {
        return string;
    }
    
    char *push_result = smArenaPush(arena, capacity);
#ifndef SM_ASSURE
    if (!push_result) {
        return string;
    }
#endif
    string.contents = push_result;
    string.capacity = capacity;
    string.length = contents_length;
    memcpy(string.contents, contents, contents_length);
    return string;
}

void smStringDeinit(smString *string) {
    string->contents = NULL;
    string->length = 0;
    string->capacity = 0;
}

void smStringClear(smString *string) {
    string->length = 0;
}

bool smStringAreContentsSame(const smString *string1, const smString *string2) {
    if (string1->length != string2->length) {
        return false;
    } 
    int compare_result = 1;
    compare_result = memcmp(string1->contents, string2->contents, string1->length);
    if (compare_result != 0) {
        return false;
    } else {
        return true;
    }
}

char smStringIndex(const smString *string, size_t index) {
    // unsure if I should put an SM_ASSURE here
    if (index >= string->length) {
        return '\0';
    } else {
        return *(string->contents + index);
    }
}

int smStringAppendCstring(smString *dest_string, const char *cstring) {
    size_t cstring_length = strlen(cstring);
#ifndef SM_ASSURE
    if ((dest_string->length + cstring_length) > dest_string->capacity) {
        return EXIT_FAILURE;
    }
#endif
    memcpy((dest_string->contents + dest_string->length), cstring, cstring_length);
    dest_string->length += cstring_length;
    return EXIT_SUCCESS;
}

int smStringAppendString(smString *dest_string, const smString *source_string) {
    return smStringAppendSubString(dest_string, source_string, 0, 0);
}

int smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

#ifndef SM_ASSURE
    if ((dest_string->length + sub_string_length) > dest_string->capacity || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return EXIT_FAILURE;
    }
#endif
    memcpy((dest_string->contents + dest_string->length), (sub_string->contents + sub_string_index), sub_string_length);
    dest_string->length += sub_string_length;
    return EXIT_SUCCESS;
}

int smStringCopyCstring(smString *dest_string , const char *cstring) {
    size_t cstring_length = strlen(cstring);
#ifndef SM_ASSURE
    if (dest_string->capacity < cstring_length) {
        return EXIT_FAILURE;
    }
#endif
    memcpy(dest_string->contents, cstring, cstring_length);
    dest_string->length = cstring_length;
    return EXIT_SUCCESS;
}

int smStringCopyString(smString *dest_string, const smString *copied_string) {
    return smStringCopySubString(dest_string, copied_string, 0, 0);
}

int smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

#ifndef SM_ASSURE
    if (dest_string->capacity < sub_string_length || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return EXIT_FAILURE;
    }
#endif
    memcpy(dest_string->contents, (sub_string->contents + sub_string_index), sub_string_length);
    dest_string->length = sub_string_length;
    return EXIT_SUCCESS;
}

int smStringWriteCstringAtIndex(smString *dest_string, size_t dest_index, const char *cstring) {
    size_t cstring_length = strlen(cstring);
#ifndef SM_ASSURE
    if ((dest_index + cstring_length) > dest_string->capacity) {
        return EXIT_FAILURE;
    }
#endif
    memcpy((dest_string->contents + dest_index), cstring, cstring_length);
    if (dest_string->length < (cstring_length + dest_index)) {
        dest_string->length = cstring_length + dest_index;
    }
    return EXIT_SUCCESS;
}
int smStringWriteStringAtIndex(smString *dest_string, size_t dest_index, const smString *source_string) {
    return smStringWriteSubStringAtIndex(dest_string, dest_index, source_string, 0, 0);
}

int smStringWriteSubStringAtIndex(smString *dest_string, size_t dest_index, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

#ifndef SM_ASSURE
    if ((dest_index + sub_string_length) > dest_string->capacity || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return EXIT_FAILURE;
    }
#endif
    memcpy((dest_string->contents + dest_index), (sub_string->contents + sub_string_index), sub_string_length);
    if (dest_string->length < (sub_string_length + dest_index)) {
        dest_string->length = sub_string_length + dest_index;
    }
    return EXIT_SUCCESS;
}

int smStringRemove(smString *string, size_t index, size_t length) {
    if (length == 0) {
        length = string->length - index;
    }

#ifndef SM_ASSURE
    if (!SM_STRING_IS_VALID_SUB_STRING(string, index, length)) {
        return EXIT_FAILURE;
    } 
#endif
    if ((index + length) == string->length) {
        string->length = index;
        return EXIT_SUCCESS;
    } else {
        memcpy((string->contents + index), (string->contents + index + length), (string->length - (index + length)));
        string->length -= length;
        return EXIT_SUCCESS;
    }
}

int smStringPush(smString *string, char character) {
#ifndef SM_ASSURE
    if ((string->length) >= (string->capacity)) {
        return EXIT_FAILURE;
    }
#endif
    *(string->contents + string->length) = character;
    string->length++;
    return EXIT_SUCCESS;
}

char smStringPop(smString *string) {
    // unsure if this should have an SM_ASSURE
    if (string->length == 0) {
        return '\0';
    } else {
        string->length--;
        return *(string->contents + string->length);
    }
}

size_t smStringFindCstring(const smString *searched_string, size_t search_index, size_t search_length, const char *cstring) {
    if (search_length == 0) {
        search_length = searched_string->length - search_index;
    }
#ifndef SM_ASSURE
    if (!SM_STRING_IS_VALID_SUB_STRING(searched_string, search_index, search_length)) {
        return SIZE_MAX;
    }
#endif
    size_t cstring_length = strlen(cstring);
#ifndef SM_ASSURE
    if (search_length < cstring_length) {
        return SIZE_MAX;
    }
#endif
    char *first_occurence = memmem(searched_string->contents + search_index, search_length, cstring, cstring_length);
#ifndef SM_ASSURE
    if (!first_occurence) {
        return SIZE_MAX;
    }
#endif
    return first_occurence - searched_string->contents;
}

size_t smStringFindString(const smString *searched_string, size_t search_index, size_t search_length, const smString *find_string) {
    return smStringFindSubString(searched_string, search_index, search_length, find_string, 0, 0);
}

size_t smStringFindSubString(const smString *searched_string, size_t search_index, size_t search_length, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

    if (search_length == 0) {
        search_length = searched_string->length - search_index;
    }
#ifndef SM_ASSURE
    if (!SM_STRING_IS_VALID_SUB_STRING(searched_string, search_index, search_length) || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return SIZE_MAX;
    } else if (search_length < sub_string_length) {
        return SIZE_MAX;
    }
#endif
    char *first_occurence = memmem(searched_string->contents + search_index, search_length, sub_string->contents + sub_string_index, sub_string_length);
#ifndef SM_ASSURE
    if (!first_occurence) {
        return SIZE_MAX;
    }
#endif
    return first_occurence - searched_string->contents;
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

smHashTable smHashTableInit(smArena *arena, size_t value_num_bytes, size_t expected_num_buckets) {
    smHashTable hash_table = {
        .buckets = 0,
        .bucket_num_bytes = 0,
        .max_num_buckets = 0,
        .num_used_buckets = 0,
    }; 

    size_t table_max_num_buckets = expected_num_buckets * 100u / SM_HASH_LOAD_FACTOR;
    size_t bytes_to_push = table_max_num_buckets * (value_num_bytes + sizeof(uint32_t));

    uintptr_t push_result = (uintptr_t)smArenaPush(arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!push_result) {
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

int smHashTableSet(smHashTable *hash_table, const void *key, size_t key_num_bytes, const void *value) {
#ifndef SM_ASSURE
    if (hash_table->num_used_buckets >= hash_table->max_num_buckets) {
        return EXIT_FAILURE;
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
    return EXIT_SUCCESS;
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

bool smIsMemZeroed(const void *mem, size_t num_bytes) {
    for (size_t i = 0; i < num_bytes; i++) {
        if (((unsigned char *)mem)[i] != (unsigned char)0) {
            return false;
        }
    }
    return true;
}
