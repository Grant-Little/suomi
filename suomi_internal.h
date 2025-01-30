#ifndef SUOMI_INTERNAL_H
#define SUOMI_INTERNAL_H

#include <stddef.h>
#include <stdbool.h>

void *smMemMem(const void *haystack, size_t haystack_length, const void *needle, size_t needle_length);
bool smIsMemZeroed(const void *mem, size_t num_bytes);

#endif
