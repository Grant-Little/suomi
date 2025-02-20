#include "../suomi.h"
#include <assert.h>

int main() {
    smError error = 0;
    smArena arena = smArenaInit(&error, 5000);
    assert(!error);

    smQueue queue_of_ints = smQueueInit(&error, &arena, sizeof(int), 100);
    assert(!error);

    int val1 = 420;
    int val2 = 69;
    int val3 = 911;

    smQueueInsert(&error, &queue_of_ints, &val1);
    smQueueInsert(&error, &queue_of_ints, &val2);
    smQueueInsert(&error, &queue_of_ints, &val3);
    assert(!error);

    int *result = smQueueRetrieve(&error, &queue_of_ints);
    assert(!error);
    assert(*result == val1);

    smArenaDeinit(&arena);
}
