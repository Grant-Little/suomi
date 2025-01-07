#ifndef SUOMI_ARENA_H
#define SUOMI_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uintptr_t start_pos;
    uintptr_t end_pos;
    uintptr_t current_pos;
} smArena;

smArena smArenaInit(size_t num_bytes);
void smArenaDeinit(smArena *arena);
void smArenaClear(smArena *arena);
void *smArenaPush(smArena *arena, size_t num_bytes);
void smArenaPop(smArena *arena, size_t num_bytes);

#endif
