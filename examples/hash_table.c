#include "../suomi_hash_table.h"
#include <assert.h>
#include <string.h>

int main() {
    smArena arena = smArenaInit(10000);
    bool result = false;

    // make a has table that stores integers, and that we expect to hold 100 integers
    smHashTable table = smHashTableInit(&arena, sizeof(int), 100);

    const char *key1 = "a string key";
    int val1 = 52;
    float key2 = 1.0f;
    int val2 = 52;

    result = smHashTableSet(&table, key1, strlen(key1), &val1);
    assert(result == true);

    result = smHashTableSet(&table, &key2, sizeof(key2), &val2);
    assert(result == true);

    assert(*(int *)smHashTableRetrieve(&table, key1, strlen(key1)) == *(int *)smHashTableRetrieve(&table, &key2, sizeof(key2)));

    return 0;
}
