#include "../suomi.h"
#include <assert.h>
#include <string.h>

int main() {
    smError error = SM_NONE;
    smArena arena = smArenaInit(&error, 10000);

    // make a has table that stores integers, and that we expect to hold 100 integers
    smHashTable table = smHashTableInit(&error, &arena, sizeof(int), 100);
    assert(error == 0);

    const char *key1 = "a string key";
    int val1 = 52;
    float key2 = 1.0f;
    int val2 = 52;

    smHashTableInsert(&error, &table, key1, strlen(key1), &val1);

    smHashTableInsert(&error, &table, &key2, sizeof(key2), &val2);
    assert(error == 0);

    assert(*(int *)smHashTableRetrieve(&table, key1, strlen(key1)) == *(int *)smHashTableRetrieve(&table, &key2, sizeof(key2)));

    return 0;
}
