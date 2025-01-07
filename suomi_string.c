#include "suomi_string.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define SM_STR_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length) (sub_string_index < sub_string->length || (sub_string_index + sub_string_length) <= sub_string->length)

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
    if (!push_result) {
        return string;
    } else {
        string.contents = push_result;
        string.capacity = capacity;
        return string;
    }
}

void smStringDeinit(smString *string) {
    string->contents = NULL;
    string->length = 0;
    string->capacity = 0;
}

void smStringClear(smString *string) {
    string->length = 0;
}

char smStringIndex(const smString *string, size_t index) {
    if (index >= string->length) {
        return '\0';
    } else {
        return *(string->contents + index);
    }
}

bool smStringAppendCstring(smString *dest_string, const char *cstring) {
    size_t cstring_length = strlen(cstring);
    if ((dest_string->length + cstring_length) > dest_string->capacity) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_string->length), cstring, cstring_length);
        dest_string->length += cstring_length;
        return true;
    }
}

bool smStringAppendString(smString *dest_string, const smString *source_string) {
    if ((dest_string->length + source_string->length) > dest_string->capacity) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_string->length), source_string->contents, source_string->length);
        dest_string->length += source_string->length;
        return true;
    }
}

bool smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if ((dest_string->length + sub_string_length) > dest_string->capacity || !SM_STR_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_string->length), (sub_string->contents + sub_string_index), sub_string_length);
        dest_string->length += sub_string_length;
        return true;
    }
}

bool smStringCopyCstring(smString *dest_string , const char *cstring) {
    size_t cstring_length = strlen(cstring);
    if (dest_string->capacity < cstring_length) {
        return false;
    } else {
        memcpy(dest_string->contents, cstring, cstring_length);
        dest_string->length = cstring_length;
        return true;
    }
}

bool smStringCopyString(smString *dest_string, const smString *copied_string) {
    if (dest_string->capacity < copied_string->length) {
        return false;
    } else {
        memcpy(dest_string->contents, copied_string->contents, copied_string->length);
        dest_string->length = copied_string->length;
        return true;
    }
}

bool smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (dest_string->capacity < sub_string_length || !SM_STR_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return false;
    } else {
        memcpy(dest_string->contents, (sub_string->contents + sub_string_index), sub_string_length);
        dest_string->length = sub_string_length;
        return true;
    }
}

bool smStringWriteCstringAtIndex(smString *dest_string, const char *cstring, size_t dest_index) {
    size_t cstring_length = strlen(cstring);
    if ((dest_index + 1 + cstring_length) > dest_string->capacity) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_index), cstring, cstring_length);
        if (dest_string->length < (cstring_length + dest_index)) {
            dest_string->length = cstring_length + dest_index;
        }
        return true;
    }
}

bool smStringWriteStringAtIndex(smString *dest_string, const smString *source_string, size_t dest_index) {
    if ((dest_index + 1 + source_string->length) > dest_string->capacity) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_index), source_string->contents, source_string->length);
        if (dest_string->length < (source_string->length + dest_index)) {
            dest_string->length = source_string->length + dest_index;
        }
        return true;
    }
}

bool smStringWriteSubStringAtIndex(smString *dest_string, const smString *sub_string, size_t dest_index, size_t sub_string_index, size_t sub_string_length) {
    if ((dest_index + 1 + sub_string_length) > dest_string->capacity || !SM_STR_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_index), (sub_string->contents + sub_string_index), sub_string_length);
        if (dest_string->length < (sub_string_length + dest_index)) {
            dest_string->length = sub_string_length + dest_index;
        }
        return true;
    }
}

bool smStringPush(smString *string, char character) {
    if ((string->length) >= (string->capacity)) {
        return false;
    } else {
        *(string->contents + string->length) = character;
        string->length++;
        return true;
    }
}

char smStringPop(smString *string) {
    if (string->length == 0) {
        return '\0';
    } else {
        string->length--;
        return *(string->contents + string->length);
    }
}
