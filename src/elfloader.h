
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
	// Index in the NAME TABLE.
	uint32_t name_index;
	// Name pointer in the NAME TABLE.
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

typedef struct {
	// Whether the file was successfully loaded.
	bool valid;
	
	// The entrypoint address.
	uint32_t entrypoint;
	
	// Number of program header segments.
	size_t    num_prog_header;
	// Program header segments.
	elf_ph_t *prog_header;
	
	// Number of section headers.
	size_t    num_sect_header;
	// Section headers.
	elf_sh_t *sect_header;
} elf_ctx_t;

// Load an ELF file into memory.
elf_ctx_t elf_interpret(FILE *fd);

#ifdef __cplusplus
}
#endif
