
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// A symbol defined or referenced by an ELF file.
typedef struct {
	// Index in the NAME TABLE.
	uint32_t name_index;
	// Copy of the name.
	char    *name;
	// The value of this symbol.
	uint32_t value;
	// The size associated with the symbol.
	uint32_t size;
	// Specifies the symbol's type.
	uint8_t  info;
	// The index of the section that this symbol is in.
	uint16_t sect_idx;
} elf_sym_t;

// A program header, which is used to load the progam for running.
typedef struct {
	// Index in the NAME TABLE.
	uint32_t name_index;
	// Copy of the name.
	char    *name;
	// Type of section.
	uint32_t type;
	// Flags bitmap.
	uint32_t flags;
	// Virtual address for sections that need loading.
	uint32_t vaddr;
	// Offset in the file image of section data.
	uint32_t offset;
	// Size in the file image.
	uint32_t file_size;
	// A related section, if any.
	uint32_t link;
	// Type dependent additional info.
	uint32_t info;
	// Alignment, must be an integer power of two.
	uint32_t alignment;
	// The size, in bytes, of an entry, for sections with fixed-size entries.
	uint32_t entry_size;
} elf_sh_t;

// A section header, which contains linking information.
typedef struct {
	// Type of the segment.
	uint32_t type;
	// Offset in the file image.
	uint32_t offset;
	// Virtual address of segment.
	uint32_t vaddr;
	// Physical address, if any.
	uint32_t paddr;
	// Size in the file image in bytes.
	uint32_t file_size;
	// Size in memory.
	uint32_t mem_size;
	// Flags bitfield.
	uint32_t flags;
	// Alignment, must be an integer power of two.
	uint32_t alignment;
} elf_ph_t;

// A complete collection of information about an ELF file.
typedef struct {
	// Whether the file was successfully interpreted.
	bool valid;
	
	// The entrypoint address.
	uint32_t   entrypoint;
	
	// Number of program header segments.
	size_t     num_prog_header;
	// Program header segments.
	elf_ph_t  *prog_header;
	
	// Number of section headers.
	size_t     num_sect_header;
	// Section headers.
	elf_sh_t  *sect_header;
	
	// Number of symbols.
	size_t     num_symbols;
	// Symbols.
	elf_sym_t *symbols;
} elf_ctx_t;

// A loaded instance of an ELF file.
typedef struct {
	// Whether the file was successfully loaded.
	bool valid;
	
	// The address offset at which it is loaded.
	// Addresses in elf_ctx_t are relative to this.
	size_t vaddr;
	
	// The memory that was allocated in which the program is loaded.
	// WARNING: This is not allocated using malloc!
	void  *memory;
} elf_loaded_t;

// Interpret an ELF file before loading.
elf_ctx_t    elf_interpret(FILE *fd);

// Load an ELF file for execution.
elf_loaded_t elf_load     (FILE *fd, elf_ctx_t *ctx);

// Get the section header with the specified name.
elf_sh_t    *elf_find_sect(elf_ctx_t *ctx, const char *name);

// Find a symbol from the table.
elf_sym_t   *elf_find_sym (elf_ctx_t *ctx, const char *name);

#ifdef __cplusplus
}
#endif
