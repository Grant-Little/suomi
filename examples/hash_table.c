#include "../suomi.h"
#include <assert.h>
#include <string.h>

int main() {
    sm_Error err = SM_NONE;
    sm_Arena arena = sm_arena_init(&err, 10000);

    // make a has table that stores integers, and that we expect to hold 100 integers
    sm_Hash_Table table = sm_hash_table_init(&err, &arena, sizeof(int), 100);
    assert(!err);

    const char *key1 = "a string key";
    int val1 = 52;
    float key2 = 1.0f;
    int val2 = 52;

    sm_hash_table_insert(&err, &table, key1, strlen(key1), &val1);

    sm_hash_table_insert(&err, &table, &key2, sizeof(key2), &val2);
    assert(!err);

    assert(*(int *)sm_hash_table_retrieve(&table, key1, strlen(key1)) == *(int *)sm_hash_table_retrieve(&table, &key2, sizeof(key2)));

    return 0;
}
