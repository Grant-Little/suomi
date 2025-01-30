//#include "../suomi_arena.h"
#include "../suomi.h"
#include <string.h>
#include <assert.h>

// imaginary data
// too large for us to put on the stack
typedef struct {
    int data[300000];
} foo;

int main() {
    // initialize an arena for the amount of data we want
    smArena arena = smArenaInit(2 * sizeof(foo));
    
    // push some memory for us to use
    foo *foo1 = smArenaPush(&arena, sizeof(foo));
    foo *foo2 = smArenaPush(&arena, sizeof(foo));

    // do whatever we need to initialize the memory
    memset((foo1->data + 250000), 'a', 10);
    memset((foo2->data + 250000), 'a', 10);

    assert(memcmp((foo1->data + 250000), (foo2->data + 250000), 100) == 0);

    // when we're done with the arena, call deinit to free the memory block
    smArenaDeinit(&arena);

    return 0;
}
