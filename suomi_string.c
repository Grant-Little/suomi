#include "suomi_string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
