
#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
// Performs an xxd-style hexdump of some memory.
void hexdump(const uint8_t *memory, size_t len, size_t cols=8);
#else
// Performs an xxd-style hexdump of some memory.
void hexdump(const uint8_t *memory, size_t len, size_t cols);
#endif

#ifdef __cplusplus
}
#endif
