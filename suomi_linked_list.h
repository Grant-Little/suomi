#ifndef SUOMI_LINKED_LIST_H
#define SUOMI_LINKED_LIST_H

#include "suomi_arena.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uintptr_t nodes_start;
    uintptr_t nodes_end;
    size_t value_num_bytes;
} smLinkedList;

smLinkedList smLinkedListInit(smArena *arena, size_t value_num_bytes, size_t max_num_nodes);
void smLinkedListDeinit(smLinkedList *linked_list);
void smLinkedListClear(smLinkedList *linked_list);

int smLinkedListInsert(smLinkedList *linked_list, void *previous_node, const void* value);
int smLinkedListDelete(smLinkedList *linked_list, void *node_to_delete);
void *smLinkedListTraverse(smLinkedList *linked_list, const void *current_node, int traverse_steps);
void *smLinkedListFind(const smLinkedList *linked_list, const void *value);
//int smLinkedListSort(smLinkedList *linked_list, void *any_node);

#endif
