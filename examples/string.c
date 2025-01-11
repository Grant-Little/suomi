#include "../suomi_string.h"
#include <assert.h>

int main() {
    smArena arena = smArenaInit(10000);
    bool result = false;

    smString string1 = smStringInit(&arena, 256);
    result = smStringAppendCstring(&string1, "contents of string1");
    assert(result == true);

    smString string2 = smStringInitWithContents(&arena, "contents of string2", 256);
    smString string3 = smStringInitWithContents(&arena, "contents of string3", 0);
    assert(smStringAreContentsSame(&string2, &string3) == false);

    size_t index = 0;
    index = smStringFindCstring(&string1, 2, 0, "string");
    assert(index == 12);

    return 0;
}
