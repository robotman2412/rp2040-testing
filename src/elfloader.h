
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ET_NONE 0x00
#define ET_REL  0x01
#define ET_EXEC 0x02
#define ET_DYN  0x03

#define SHT_PROGBITS  1
#define SHT_SYMTAB    2
#define SHT_STRTAB    3
#define SHT_RELA      4
#define SHT_HASH      5
#define SHT_DYNAMIC   6
#define SHT_NOTE      7
#define SHT_NOBITS    8
#define SHT_REL       9
#define SHT_SHLIB    10
#define SHT_DYNSYM   11

typedef struct elf_ctx elf_ctx_t;

// A program header, which is used to load the progam for running.
typedef struct {
	// The parent ELF context.
	elf_ctx_t *parent;
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
	// The parent ELF context.
	elf_ctx_t *parent;
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

// A symbol defined or referenced by an ELF file.
typedef struct {
	// The parent ELF context.
	elf_ctx_t *parent;
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
	// The symbol's binding.
	uint8_t  binding;
	// The symbol's type.
	uint8_t  type;
	// The index of the section that this symbol is in.
	uint16_t sect_idx;
} elf_sym_t;

// A complete collection of information about an ELF file.
struct elf_ctx {
	// Whether the file was successfully interpreted.
	bool valid;
	
	// Whether the file is encoded in little endian.
	bool is_le;
	// The type of ELF file we're dealing with.
	uint32_t type;
	// The machine type the ELF file is compiled for.
	uint32_t machine;
	
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
};

// A loaded to memory instance of a symbol.
typedef struct {
	// The symbol context this was loaded from.
	elf_sym_t *parent;
	// The resolved virtual address.
	uint32_t   vaddr;
	// The global offset table address for this symbol, if any.
	uint32_t   got_addr;
	// Whether this symbol appears in the global offset table.
	bool       got_present;
} elf_load_sym_t;

// A loaded to memory instance of a section.
typedef struct {
	// The section this was loaded from.
	elf_sh_t *parent;
	// The resolved virtual address.
	uint32_t  vaddr;
	// Whether this sections needs to be loaded.
	bool      do_load;
} elf_load_sh_t;

// A loaded instance of an ELF file.
typedef struct {
	// Whether the file was successfully loaded.
	bool valid;
	
	// The context from which this was loaded.
	elf_ctx_t *ctx;
	
	// The number of sections loaded.
	size_t          num_sections;
	// The loaded sections.
	elf_load_sh_t  *sections;
	
	// The number of symbols loaded.
	size_t          num_symbols;
	// The loaded symbols.
	elf_load_sym_t *symbols;
	
	// The address offset at which it is loaded.
	// Addresses in elf_ctx_t are relative to this.
	size_t vaddr;
	// The memory that was allocated in which the program is loaded.
	// WARNING: This is not allocated using malloc!
	void  *memory;
} elf_loaded_t;

// A linkage of multiple ELF files.
typedef struct {
	// Whether the linkage was successfully loaded.
	bool          valid;
	
	// The amount of loaded ELF files.
	size_t        num_loaded;
	// Produced loaded ELF files.
	elf_loaded_t *loaded;
	
	// The address of the global offset table.
	size_t        got_addr;
	// The address offset at which it is loaded.
	// Addresses in elf_ctx_t are relative to this.
	size_t        vaddr;
	// The memory that was allocated in which the program is loaded.
	// WARNING: This is not allocated using malloc!
	void         *memory;
} elf_link_t;

// A relocation entry.
typedef struct {
	// The section to which to apply this.
	elf_load_sh_t *target;
	// The loaded file to which to apply this.
	elf_loaded_t  *ctx;
	// The offset in the target section at which to start.
	uint32_t       offset;
	// The symbol index used for reference.
	uint32_t       symbol;
	// The type of relocation performed.
	uint8_t        type;
	// A constant which helps with the magic.
	uint32_t       addend;
} elf_reloc_t;

// Interpret an ELF file before loading.
elf_ctx_t    elf_interpret(FILE *fd);

// Load an ELF file into dynamically allocated memory.
elf_loaded_t elf_load_dyn(FILE *fd, elf_ctx_t *ctx);
// Load an ELF file into preallocated memory.
elf_loaded_t elf_load(FILE *fd, elf_ctx_t *ctx, size_t paddr, size_t cap);

// Advanced loading method.
// If an ELF in to_load has valid=false, it will try to interpret this file.
elf_link_t   elf_linked_load(size_t num_to_load, elf_ctx_t **to_load, FILE **to_load_fds, size_t num_loaded, elf_loaded_t **loaded);

// Get the section header with the specified name.
elf_sh_t    *elf_find_sect(elf_ctx_t *ctx, const char *name);

// Find a symbol from the table.
elf_sym_t   *elf_find_sym(elf_ctx_t *ctx, const char *name);

// Get the loaded address of a symbol.
// Returns NULL when not found.
void        *elf_adrof_sym_simple(elf_loaded_t *loaded, const char *name, bool allow_object, bool allow_function);
// Get the loaded address of a symbol.
// Returns NULL when not found.
void        *elf_adrof_sym(elf_link_t *linkage, const char *name, bool allow_object, bool allow_function);

#ifdef __cplusplus
}
#endif
