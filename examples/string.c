#include "../suomi.h"
#include <assert.h>

int main() {
    smError error = SM_NONE;
    smArena arena = smArenaInit(&error, 10000);

    smString string1 = smStringInit(&error, &arena, 256);
    smStringAppendCstring(&error, &string1, "contents of string1");
    assert(error == SM_NONE);

    smString string2 = smStringInitWithContents(&error, &arena, "contents of string2", 256);
    smString string3 = smStringInitWithContents(&error, &arena, "contents of string3", 0);
    assert(smStringAreContentsSame(&string2, &string3) == false);
    assert(error == SM_NONE);

    size_t index = 0;
    index = smStringFindCstring(&string1, 2, 0, "string");
    assert(index == 12);

    smString string4 = smStringInitWithContents(&error, &arena, "stringaaaa", 256);
    smString string5 = smStringInitWithContents(&error, &arena, "bbbbstring", 256);

    smStringRemove(&error, &string4, 6, 4);
    smStringRemove(&error, &string5, 0, 4);
    assert(error == SM_NONE);

    assert(smStringAreContentsSame(&string4, &string5) == true);

    return 0;
}
