#include "suomi_hash_table.h"
#include <stdio.h>
#include <string.h>

int main() {
    smArena arena = smArenaInit(10000);
    bool result = false;
    const char *key = "some key";
    int value = 69;

    smHashTable table = smHashTableInit(&arena, sizeof(int), 100);

    result = smHashTableSet(&table, key, strlen(key), &value);
    if (!result) {
        printf("failed to set table entry\n");
    }

    int new_value = *(int *)smHashTableRetrieve(&table, key, strlen(key));

    printf("retrieved value: %d\n", new_value);

    smArenaDeinit(&arena);
    return 0;
}
