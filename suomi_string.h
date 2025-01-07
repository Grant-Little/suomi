#ifndef SUOMI_STRING_H
#define SUOMI_STRING_H

#include "suomi_arena.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char *contents;
    size_t length;
    size_t capacity;
} smString;

// displaying strings
void smStringPuts(const smString *string);

// creating and destroying strings
smString smStringInit(smArena *arena, size_t capacity);
void smStringDeinit(smString *string);
void smStringClear(smString *string);

// index string
char smStringIndex(const smString *string, size_t index); //positive indices, return '\0' on invalid index

// modifying strings
bool smStringAppendCstring(smString *dest_string, const char *cstring);
bool smStringAppendString(smString *dest_string, const smString *source_string);
bool smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
bool smStringCopyCstring(smString *dest_string , const char *cstring);
bool smStringCopyString(smString *dest_string, const smString *copied_string);
bool smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length);
bool smStringWriteCstringAtIndex(smString *dest_string, const char *cstring, size_t dest_index);
bool smStringWriteStringAtIndex(smString *dest_string, const smString *source_string, size_t dest_index);
bool smStringWriteSubStringAtIndex(smString *dest_string, const smString *sub_string, size_t dest_index, size_t sub_string_index, size_t sub_string_length);
bool smStringPush(smString *string, char character);
char smStringPop(smString *string);

// search for first occurence, provide index and length to search, return SIZE_MAX if not found, 0 length is taken as dest_string->length
size_t smStringFindCstring(const smString *search_string, const char *cstring, size_t search_index, size_t search_length, bool reverse_search);
size_t smStringFindString(const smString *search_string, const smString *find_string, size_t search_index, size_t search_length, bool reverse_search);
size_t smStringFindSubString(const smString *search_string, const smString *sub_string, size_t search_index, size_t search_length, size_t sub_string_index, size_t sub_string_length, bool reverse_search);

/*
check if string is alphanumeric etc
have flags like
SM_STR_NUMERIC
SM_STR_ALPHABETICAL

search on index range, 0 and 0 would be the whole string
*/

#define SM_STR_NUMERIC 0x001 // 48-57
#define SM_STR_LOWERCASE 0x0002 // 97-122
#define SM_STR_UPPERCASE 0x0004 // 65-90
#define SM_SIR_ALPHABETICAL 0x0006
#define SM_STR_ALPHANUMERIC 0x0007
#define SM_STR_WHITESPACE 0x0008 // 32, 9-12
#define SM_STR_PUNCTUATION 0x0010 // 32-64, 91-96, 123-126
#define SM_STR_BRACKETS 0x0020 // 40, 41, 60, 62, 91, 93, 123, 125
#define SM_STR_ARITHMETIC 0x0040 // 42, 43, 45, 47
#define SM_STR_ASCII_CONTROL 0x0080 // 0-32, 127
#define SM_STR_ASCII_PRINTABLE 0x0100 // 33-126

bool smStringIsCategory(const smString *string, unsigned short string_categories, size_t index, size_t length);

#endif
