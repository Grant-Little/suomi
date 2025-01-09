#include "suomi_string.h"
#include <string.h>
#include <stdio.h>

#define SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length) (sub_string_index < sub_string->length || (sub_string_index + sub_string_length) <= sub_string->length)

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

smString smStringInitWithContents(smArena *arena, const char *contents, size_t capacity) {
    smString string = {
        .contents = NULL,
        .length = 0,
        .capacity = 0,
    };
    
    size_t contents_length = strlen(contents);
    if (capacity < contents_length) {
        capacity = contents_length;
    }

    if (contents_length > capacity) {
        return string;
    }
    
    char *push_result = smArenaPush(arena, capacity);
    if (!push_result) {
        return string;
    } else {
        string.contents = push_result;
        string.capacity = capacity;
        string.length = contents_length;
        memcpy(string.contents, contents, contents_length);
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
    return smStringAppendSubString(dest_string, source_string, 0, 0);
}

bool smStringAppendSubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

    if ((dest_string->length + sub_string_length) > dest_string->capacity || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
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
    return smStringCopySubString(dest_string, copied_string, 0, 0);
}

bool smStringCopySubString(smString *dest_string, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

    if (dest_string->capacity < sub_string_length || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return false;
    } else {
        memcpy(dest_string->contents, (sub_string->contents + sub_string_index), sub_string_length);
        dest_string->length = sub_string_length;
        return true;
    }
}

bool smStringWriteCstringAtIndex(smString *dest_string, size_t dest_index, const char *cstring) {
    size_t cstring_length = strlen(cstring);
    if ((dest_index + cstring_length) > dest_string->capacity) {
        return false;
    } else {
        memcpy((dest_string->contents + dest_index), cstring, cstring_length);
        if (dest_string->length < (cstring_length + dest_index)) {
            dest_string->length = cstring_length + dest_index;
        }
        return true;
    }
}
bool smStringWriteStringAtIndex(smString *dest_string, size_t dest_index, const smString *source_string) {
    return smStringWriteSubStringAtIndex(dest_string, dest_index, source_string, 0, 0);
}

bool smStringWriteSubStringAtIndex(smString *dest_string, size_t dest_index, const smString *sub_string, size_t sub_string_index, size_t sub_string_length) {
    if (sub_string_length == 0) {
        sub_string_length = sub_string->length - sub_string_index;
    }

    if ((dest_index + sub_string_length) > dest_string->capacity || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
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

size_t smStringFindCstring(const smString *searched_string, size_t search_index, size_t search_length, const char *cstring) {
    if (search_length == 0) {
        search_length = searched_string->length - search_index;
    }

    if (!SM_STRING_IS_VALID_SUB_STRING(searched_string, search_index, search_length)) {
        return SIZE_MAX;
    }

    size_t cstring_length = strlen(cstring);
    if (search_length < cstring_length) {
        return SIZE_MAX;
    }

    char *first_occurence = memmem(searched_string->contents + search_index, search_length, cstring, cstring_length);
    if (!first_occurence) {
        return SIZE_MAX;
    } else {
        return first_occurence - searched_string->contents;
    }
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

    if (!SM_STRING_IS_VALID_SUB_STRING(searched_string, search_index, search_length) || !SM_STRING_IS_VALID_SUB_STRING(sub_string, sub_string_index, sub_string_length)) {
        return SIZE_MAX;
    }

    if (search_length < sub_string_length) {
        return SIZE_MAX;
    }

    char *first_occurence = memmem(searched_string->contents + search_index, search_length, sub_string->contents + sub_string_index, sub_string_length);
    if (!first_occurence) {
        return SIZE_MAX;
    } else {
        return first_occurence - searched_string->contents;
    }
}

bool smStringIsCategory(const smString *string, size_t index, size_t length, uint16_t string_categories) {
    if (length == 0) {
        length = string->length - index;
    }

    if (!SM_STRING_IS_VALID_SUB_STRING(string, index, length)) {
        return false;
    }

    bool is_category = false;

#define CHR string->contents[i]
    if (SM_STRING_NUMERIC & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR < 48 || CHR > 57) {
                return false;
            }
        }
    }

    if (SM_STRING_LOWERCASE & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR < 97 || CHR > 122) {
                return false;
            }
        }
    }

    if (SM_STRING_UPPERCASE & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR < 65 || CHR > 90) {
                return false;
            }
        }
    }

    if (SM_STRING_WHITESPACE & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR != 32 && (CHR < 9 || CHR > 12)) {
                return false;
            }
        }
    }

    if (SM_STRING_PUNCTUATION & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if ((CHR < 32 || CHR > 64) && (CHR < 91 || CHR > 96) && (CHR < 123 || CHR > 126)) {
                return false;
            }
        }

    }

    if (SM_STRING_BRACKETS & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR != 40 && CHR != 41 && CHR != 60 && CHR != 62 && CHR != 91 && CHR != 93 && CHR != 123 && CHR != 125) {
                return false;
            }
        }
    }
    
    if (SM_STRING_BRACKETS & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR != 42 && CHR != 43 && CHR != 45 && CHR != 47) {
                return false;
            }
        }
    }

    if (SM_STRING_BRACKETS & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if ((CHR < 0 || CHR > 32) && CHR != 127) {
                return false;
            }
        }
    }

    if (SM_STRING_BRACKETS & string_categories) {
        is_category = true;
        for (size_t i = index; i < length; i++) {
            if (CHR < 33 || CHR > 126) {
                return false;
            }
        }
    }
#undef CHR

    if (is_category == false) {
        return false;
    } else {
        return true;
    }
}
