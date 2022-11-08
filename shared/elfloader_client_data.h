
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Symbol resolver callback.
// Returns NULL when not found.
typedef void *(*elf_client_resolver_t)(void *ctx, const char *name, bool allow_object, bool allow_function);

// Abort callback.
// Never returns.
typedef void (*elf_client_abort_t)();

// Data passed to the client _entry function.
typedef struct {
	// Context for the resolver callback.
	void *resolver_ctx;
	
	// Symbol resolver callback.
	// Returns NULL when not found.
	elf_client_resolver_t resolver;
	
	// Abort callback.
	// Never returns.
	elf_client_abort_t abort;
} elf_client_data_t;

// Function pointer format for the client _entry function.
typedef void (*elf_client_entry_t)(elf_client_data_t client_data);
