#include "../suomi_string.h"
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

    return 0;
}
