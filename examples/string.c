//#include "../suomi_string.h"
#include "../suomi.h"
#include <assert.h>

int main() {
    smArena arena = smArenaInit(10000);
    int exit;

    smString string1 = smStringInit(&arena, 256);
    exit = smStringAppendCstring(&string1, "contents of string1");
    assert(exit == 0);

    smString string2 = smStringInitWithContents(&arena, "contents of string2", 256);
    smString string3 = smStringInitWithContents(&arena, "contents of string3", 0);
    assert(smStringAreContentsSame(&string2, &string3) == false);

    size_t index = 0;
    index = smStringFindCstring(&string1, 2, 0, "string");
    assert(index == 12);

    smString string4 = smStringInitWithContents(&arena, "stringaaaa", 256);
    smString string5 = smStringInitWithContents(&arena, "bbbbstring", 256);

    exit = smStringRemove(&string4, 6, 4);
    assert(exit == 0);
    exit = smStringRemove(&string5, 0, 4);
    assert(exit == 0);

    assert(smStringAreContentsSame(&string4, &string5) == true);

    return 0;
}
