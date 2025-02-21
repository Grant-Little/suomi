#include "../suomi.h"
#include <assert.h>

int main() {
    sm_Error err = 0;
    sm_Arena arena = sm_arena_init(&err, 5000);
    assert(!err);

    sm_Queue queue_of_ints = sm_queue_init(&err, &arena, sizeof(int), 100);
    assert(!err);

    int val1 = 420;
    int val2 = 69;
    int val3 = 911;

    sm_queue_insert(&err, &queue_of_ints, &val1);
    sm_queue_insert(&err, &queue_of_ints, &val2);
    sm_queue_insert(&err, &queue_of_ints, &val3);
    assert(!err);

    int *result = sm_queue_retrieve(&err, &queue_of_ints);
    assert(!err);
    assert(*result == val1);

    sm_arena_deinit(&arena);
}
