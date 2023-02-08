
#include <stddef.h>

typedef struct {
	char *name;
	void *pointer;
} abi_entry_t;

extern const abi_entry_t abi_lut[];
extern const size_t abi_lut_len;
