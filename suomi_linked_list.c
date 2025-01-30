#include "suomi_linked_list.h"
#include "suomi_internal.h"
#include <string.h>
#include <stdbool.h>

#define SM_LL_NODE_SIZE(linked_list) (linked_list->value_num_bytes + 2 * sizeof(uintptr_t))
#define SM_LL_NODE_BACK_PTR(linked_list, node) (*((void *)((uintptr_t)node + linked_list->value_num_bytes)))
#define SM_LL_NODE_NEXT_PTR(linked_list, node) (*((void *)((uintptr_t)node + linked_list->value_num_bytes + sizeof(uintptr_t))))

bool smLinkedListIsNodeSlotEmpty(const smLinkedList *linked_list, const void *node_slot);
void *smLinkedListFindFirstEmptySlot(const smLinkedList *linked_list);

smLinkedList smLinkedListInit(smArena *arena, size_t value_num_bytes, size_t max_num_nodes) {
    smLinkedList linked_list = {
        .nodes_start = 0,
        .nodes_end = 0,
        .value_num_bytes = 0,
    };

    size_t bytes_to_push = max_num_nodes * (value_num_bytes + 2 * sizeof(uintptr_t));

    uintptr_t push_result = (uintptr_t)smArenaPush(arena, bytes_to_push);
#ifndef SM_ASSURE
    if (!push_result) {
        return linked_list;
    }
#endif
    memset((void *)push_result, bytes_to_push);
    linked_list.nodes_start = push_result;
    linked_list.nodes_end = push_result + bytes_to_push;
    linked_list.value_num_bytes = value_num_bytes;
    return linked_list;
}

void smLinkedListDeinit(smLinkedList *linked_list) {
    linked_list->nodes_start = 0;
    linked_list->nodes_end = 0;
    linked_list->values_num_bytes = 0;
}

void smLinkedListClear(smLinkedList *linked_list) {
    memset((void *)linked_list->nodes_start, 0, linked_list->nodes_start - linked_list->nodes_end);
}

int smLinkedListInsert(smLinkedList *linked_list, void *previous_node, const void* value) {
    void *empty_slot = smLinkedListFindFirstEmptySlot(linked_list);
#ifndef SM_ASSURE
    if (!empty_slot) {
        return EXIT_FAILURE;
    }
#endif
    memcpy(empty_slot, value, linked_list->value_num_bytes);
    if (!previous_node) {
        return EXIT_SUCCESS;
    }
    SM_LL_NODE_BACK_PTR(linked_list, empty_slot) = previous_node;
    void *next_node = SM_LL_NODE_NEXT_PTR(linked_list, previous_node);
    if (next_node) {
        SM_LL_NODE_BACK_PTR(linked_list, next_node) = empty_slot;
        SM_LL_NODE_NEXT_PTR(linked_list, empty_slot) = next_node;
    }
    SM_LL_NODE_NEXT_PTR(linked_list, previous_node) = empty_slot;
    return EXIT_SUCCESS;
}

int smLinkedListDelete(smLinkedList *linked_list, void *node_to_delete) {
    previous_node = SM_LL_NODE_BACK_PTR(linked_list, node_to_delete);
    next_node = SM_LL_NODE_NEXT_PTR(linked_list, node_to_delete);
    if (!previous_node && !next_node) {
        return EXIT_FAILURE;
    } else if (!previous_node) {
        SM_LL_NODE_BACK_PTR(linked_list, next_node) = NULL;
    } else if (!next_node) {
        SM_LL_NODE_NEXT_PTR(linked_list, previous_node) = NULL;
    } else {
        SM_LL_NODE_BACK_PTR(linked_list, next_node) = previous_node;
        SM_LL_NODE_NEXT_PTR(linked_list, previous_node) = next_node;
    }
    memset(node_to_delete, 0, SM_LL_NODE_SIZE(linked_list));
    return EXIT_SUCCESS;
}

void *smLinkedListTraverse(smLinkedList *linked_list, const void *current_node, int traverse_steps) {
    const void *return_node = current_node;
    const void *check_node = NULL;
    if (traverse_steps == 0) {
        return return_node;
    } else if (traverse_steps > 0) {
        for (int i = 0, i < traverse_steps, i++) {
            check_node = SM_LL_NODE_NEXT_PTR(linked_list, return_node);
            if (!check_node) {
                return return_node;
            }
            return_node = check_node;
        }
    } else {
        for (int i = 0, i > traverse_steps, i--) {
            check_node = SM_LL_NODE_BACK_PTR(linked_list, return_node);
            if (!check_node) {
                return return_node;
            }
            return_node = check_node;
        }
    }
    return return_node;
}

void *smLinkedListFind(const smLinkedList *linked_list, const void *value) {
    size_t max_search_length = (linked_list->nodes_end - linked_list->nodes_start) / SM_LL_NODE_SIZE(linked_list);
    uintptr_t current_node = linked_list->nodes_start;
    for (size_t i = 0; i < max_search_length; i ++) {
        if (!memcmp(value, current_node, linked_list->value_num_bytes)) {
            return current_node;
        }
        current_node += SM_LL_NODE_SIZE(linked_list);
    }
    return NULL;
}

bool smLinkedListIsNodeSlotEmpty(const smLinkedList *linked_list, const void *node_slot) {
    return smIsMemZeroed(node_slot, SM_LL_NODE_SIZE(linked_list));
}

void *smLinkedListFindFirstEmptySlot(const smLinkedList *linked_list) {
    uintptr_t current_node = linked_list->nodes_start;
    for (size_t i = 0; current_node < linked_list->nodes_end; i++) {
        if (smIsMemZeroed(current_node, SM_LL_NODE_SIZE(linked_list))) {
            return (void *)current_node;
        }
        current_node += SM_LL_NODE_SIZE(linked_list);
    }

    return NULL;
}
