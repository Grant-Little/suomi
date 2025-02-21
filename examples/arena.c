#include "../suomi.h"
#include <string.h>
#include <assert.h>

// imaginary data
// too large for us to put on the stack
typedef struct {
    int data[300000];
} foo;

int main() {
    sm_Error err = SM_NONE;
    // initialize an arena for the amount of data we want
    sm_Arena arena = sm_arena_init(&err, 2 * sizeof(foo));
    
    // push some memory for us to use
    foo *foo1 = sm_arena_push(&err, &arena, sizeof(foo));
    foo *foo2 = sm_arena_push(&err, &arena, sizeof(foo));

    // do whatever we need to initialize the memory
    memset((foo1->data + 250000), 'a', 10);
    memset((foo2->data + 250000), 'a', 10);

    assert(memcmp((foo1->data + 250000), (foo2->data + 250000), 100) == 0);
    assert(!err);

    // when we're done with the arena, call deinit to free the memory block
    sm_arena_deinit(&arena);

    return 0;
}
