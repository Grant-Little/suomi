#ifndef SUOMI_STRING_H
#define SUOMI_STRING_H

#include "suomi_arena.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

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
bool smStringAppendCstring(smString *dest_string, const char *cstring);
bool smStringAppendString(smString *dest_string, const smString *source_string);
bool smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
bool smStringCopyCstring(smString *dest_string , const char *cstring);
bool smStringCopyString(smString *dest_string, const smString *copied_string);
bool smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
bool smStringWriteCstringAtIndex(smString *dest_string, size_t dest_index, const char *cstring);
bool smStringWriteStringAtIndex(smString *dest_string, size_t dest_index, const smString *source_string);
bool smStringWriteSubStringAtIndex(smString *dest_string, size_t dest_index, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
bool smStringPush(smString *string, char character);
char smStringPop(smString *string);

// search for first occurence, provide index and length to search, return SIZE_MAX if not found, 0 search_length is taken as searched_string->length
// uses memmem which is not widely supported
// potentially write own implementation of memmem to use if memmem not defined
size_t smStringFindCstring(const smString *searched_string, size_t search_index, size_t search_length, const char *cstring);
size_t smStringFindString(const smString *searched_string, size_t search_index, size_t search_length, const smString *find_string);
size_t smStringFindSubString(const smString *searched_string, size_t search_index, size_t search_length, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);

#endif
