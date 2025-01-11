#include "suomi_arena.h"
#include <stdlib.h>

smArena smArenaInit(size_t num_bytes) {
    smArena arena = {
        .start_pos = 0,
        .end_pos = 0,
        .current_pos = 0,
    };

    arena.start_pos = (uintptr_t)malloc(num_bytes);
    if (!arena.start_pos) {
        return arena;
    }
    arena.end_pos = arena.start_pos + num_bytes;
    arena.current_pos = arena.start_pos;

    return arena;
}

void smArenaDeinit(smArena *arena) {
    free((void *)(arena->start_pos));
    arena->start_pos = 0;
    arena->end_pos = 0;
    arena->current_pos = 0;
}

void smArenaClear(smArena *arena) {
    arena->current_pos = arena->start_pos;
}

void *smArenaPush(smArena *arena, size_t num_bytes) {
    if ((arena->current_pos + num_bytes) <= arena->end_pos) {
        arena->current_pos += num_bytes;
        return (void *)(arena->current_pos - num_bytes);
    } else {
        return NULL;
    }
}

void smArenaPop(smArena *arena, size_t num_bytes) {
    arena->current_pos -= num_bytes;
    if (arena->current_pos < arena->start_pos) {
        arena->current_pos = arena->start_pos;
    }
}
