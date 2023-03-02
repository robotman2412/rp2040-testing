
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Allocate `__size` bytes of memory.
// Returns memory, or NULL if not enough is available.
void *malloc(size_t __size);
// Free a previously allocated block of memory.
// Does nothing if `__mem` is NULL.
void free(void *__mem);
// Allocate an array of `__size` elements of `__nmemb` each.
// Returns memory, or NULL if not enough is available.
void *calloc(size_t __nmemb, size_t __size);
// Reallocates `__mem` to be `__size` big.
// Moves data too if applicable.
// Returns memory, or NULL if not enough is available.
void *realloc(void *__mem, size_t __size);

// Determines how many allocatable bytes there are in total.
size_t meminfo_total();
// Determines how many bytes of memory is in use.
size_t meminfo_inuse();
// Determines how many bytes of memory can be allocated.
// Identical to `meminfo_total() - meminfo_inuse()`.
size_t meminfo_free();

#ifdef __cplusplus
}
#endif
