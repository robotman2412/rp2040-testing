
#include "elfloader.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <pico/stdlib.h>

// A magic tester for multi boits
static bool _elf_expect(FILE *fd, size_t len, const void *magic) {
	uint8_t tmp[len];
	size_t read = fread(tmp, 1, len, fd);
	if (read != len) return false;
	return !memcmp(tmp, magic, len);
}
#define EXPECT(count, magic) do { if (!_elf_expect(fd, count, magic)) goto error; } while(0)

// A INTEGER READING DEVICE
static bool _elf_readint(FILE *fd, uint64_t *out, size_t size, bool is_signed, bool is_le) {
	uint64_t tmp = 0;
	
	if (is_le) {
		for (size_t i = 0; i < size; i++) {
			int c = fgetc(fd);
			if (c == EOF) return false;
			tmp |= (uint64_t) (c & 0xff) << (i*8);
		}
	} else {
		for (size_t i = 0; i < size; i++) {
			int c = fgetc(fd);
			if (c == EOF) return false;
			tmp |= c << ((size-i-1)*8);
		}
	}
	
	if (is_signed && (tmp >> (size*8-1))) {
		for (size_t i = size; i < 8; i++) {
			tmp |= 0xffllu << (i*8);
		}
	}
	
	*out = tmp;
	return true;
}
#define READUINT(dest, size) do { uint64_t tmp; if (!_elf_readint(fd, &tmp, size, false, is_little_endian)) goto error; (dest) = tmp; } while(0)

// SKIPS PADDING.
static bool _elf_skip(FILE *fd, size_t len) {
	long pre = ftell(fd);
	fseek(fd, len, SEEK_CUR);
	return ftell(fd) - pre == len;
}
#define SKIP(size) do { if (!_elf_skip(fd, (size))) goto error; } while(0)

// SEEKD LOKATON.
static bool _elf_seek(FILE *fd, size_t idx) {
	fseek(fd, idx, SEEK_SET);
	return ftell(fd) == idx;
}
#define SEEK(idx) do { if (!_elf_seek(fd, (idx))) goto error; } while(0)

// READ BIANIER.
#define READ(ptr, len) do { if (fread(ptr, 1, len, fd) != len) goto error; } while(0)

// ALIGNMENT WITH MEM ALL O C.
static void *aligned_alloc(size_t size, size_t alignment) {
	// Get some memory.
	size_t real_size = size + alignment + sizeof(size_t);
	size_t real_mem  = (size_t) malloc(real_size);
	
	// Add alignment.
	size_t min_offs  = real_mem + sizeof(size_t);
	size_t mem       = min_offs + alignment - (min_offs % alignment);
	
	// Add INFO thingy.
	*(size_t *) (mem - sizeof(size_t)) = real_mem;
	return (void *) mem;
}

// FREE FROM THE aligned_alloc.
static void aligned_free(void *memory) {
	void *real = *(void **) ((size_t) memory - sizeof(size_t));
	free(real);
}


static const uint8_t elf_magic[4] = { 0x7f, 'E', 'L', 'F' };



// Interpret an ELF file before loading.
// Unless `needs_extra_info` is true, this will only load the bare minimum:
// Program headers and the entry vector.
elf_ctx_t elf_interpret(FILE *fd, bool needs_extra_info) {
	elf_ctx_t ctx;
	ctx.prog_header     = NULL;
	ctx.num_prog_header = 0;
	ctx.sect_header     = NULL;
	ctx.num_sect_header = 0;
	ctx.symbols         = NULL;
	ctx.num_symbols     = 0;
	char *name_tmp = NULL;
	
	// Check magic.
	EXPECT(4, elf_magic);
	
	// Check 32 bit type.
	EXPECT(1, (const char[1]){1});
	
	// Read endianness.
	int is_little_endian = true;
	READUINT(is_little_endian, 1);
	is_little_endian = is_little_endian == 1;
	ctx.is_le = is_little_endian;
	uint8_t version;
	READUINT(version, 1);
	#ifdef ENABLE_DEBUG_LOGS
	printf("ELF V%d %s endian\n", version, is_little_endian ? "little" : "big");
	#endif
	
	// Check target type.
	uint8_t osabi, abiver;
	READUINT(osabi,  1);
	READUINT(abiver, 1);
	#ifdef ENABLE_DEBUG_LOGS
	printf("OSABI %02x v%d\n", osabi, abiver);
	#endif
	
	// Skip padding.
	SKIP(7);
	
	// Check machine type.
	uint16_t type, machine;
	READUINT(type,    2);
	READUINT(machine, 2);
	#ifdef ENABLE_DEBUG_LOGS
	printf("ELF type %04x\n", type);
	printf("ISA type %02x\n", machine);
	#endif
	ctx.type    = type;
	ctx.machine = machine;
	
	// There is another verion???
	SKIP(4);
	
	// Offsets.
	uint32_t prog_header_offs, sect_header_offs;
	READUINT(ctx.entrypoint, 4);
	#ifdef ENABLE_DEBUG_LOGS
	printf("Entrypoint %08x\n", ctx.entrypoint);
	#endif
	READUINT(prog_header_offs, 4);
	READUINT(sect_header_offs, 4);
	
	// Architecture-specific flags.
	uint32_t arch_flags;
	READUINT(arch_flags, 4);
	
	// Size, in bytes, of the ELF header.
	uint32_t header_size;
	READUINT(header_size, 2);
	
	// Program header.
	uint32_t ph_ent_size;
	READUINT(ph_ent_size,         2);
	READUINT(ctx.num_prog_header, 2);
	#ifdef ENABLE_DEBUG_LOGS
	printf("%u prog headers (%u each, starting at %08x)\n", ctx.num_prog_header, ph_ent_size, prog_header_offs);
	#endif
	
	// Section header.
	uint32_t sh_ent_size, sh_name_idx;
	READUINT(sh_ent_size,         2);
	READUINT(ctx.num_sect_header, 2);
	READUINT(sh_name_idx,         2);
	#ifdef ENABLE_DEBUG_LOGS
	printf("%u sect headers (%u each, starting at %08x)\n", ctx.num_sect_header, sh_ent_size, sect_header_offs);
	printf("Name section index %d\n", sh_name_idx);
	printf("\n");
	#endif
	
	// Read program headers.
	ctx.prog_header = malloc(sizeof(elf_ph_t) * ctx.num_prog_header);
	if (!ctx.prog_header) goto error;
	
	for (size_t i = 0; i < ctx.num_prog_header; i++) {
		SEEK(prog_header_offs + i * ph_ent_size);
		READUINT(ctx.prog_header[i].type,      4);
		READUINT(ctx.prog_header[i].offset,    4);
		READUINT(ctx.prog_header[i].vaddr,     4);
		READUINT(ctx.prog_header[i].paddr,     4);
		READUINT(ctx.prog_header[i].file_size, 4);
		READUINT(ctx.prog_header[i].mem_size,  4);
		READUINT(ctx.prog_header[i].flags,     4);
		READUINT(ctx.prog_header[i].alignment, 4);
		#ifdef ENABLE_DEBUG_LOGS
		printf("Prog header %zu:\n", i);
		printf("Type:      %02x\n", ctx.prog_header[i].type);
		printf("Offset:    %08x\n", ctx.prog_header[i].offset);
		printf("Virt addr: %08x\n", ctx.prog_header[i].vaddr);
		printf("Phys addr: %08x\n", ctx.prog_header[i].paddr);
		printf("File size: %08x\n", ctx.prog_header[i].file_size);
		printf("Mem size:  %08x\n", ctx.prog_header[i].mem_size);
		printf("Flags:     %08x\n", ctx.prog_header[i].flags);
		printf("Alignment: %08x\n", ctx.prog_header[i].alignment);
		printf("\n");
		#endif
	}
	
	// Read section headers.
	ctx.sect_header = malloc(sizeof(elf_sh_t) * ctx.num_sect_header);
	if (!ctx.sect_header) goto error;
	memset(ctx.sect_header, 0, sizeof(elf_sh_t) * ctx.num_sect_header);
	
	for (size_t i = 0; i < ctx.num_sect_header; i++) {
		SEEK(sect_header_offs + i * sh_ent_size);
		ctx.sect_header[i].name = NULL;
		READUINT(ctx.sect_header[i].name_index, 4);
		READUINT(ctx.sect_header[i].type,       4);
		READUINT(ctx.sect_header[i].flags,      4);
		READUINT(ctx.sect_header[i].vaddr,      4);
		READUINT(ctx.sect_header[i].offset,     4);
		READUINT(ctx.sect_header[i].file_size,  4);
		READUINT(ctx.sect_header[i].link,       4);
		READUINT(ctx.sect_header[i].info,       4);
		READUINT(ctx.sect_header[i].alignment,  4);
		READUINT(ctx.sect_header[i].entry_size, 4);
	}
	
	// Verify validity of names section.
	if (sh_name_idx >= ctx.num_sect_header) goto error;
	elf_sh_t *name_sh = &ctx.sect_header[sh_name_idx];
	
	// Allocate memory to buffer names section.
	char *sh_name_tmp = malloc(name_sh->file_size + 1);
	if (!sh_name_tmp) goto error;
	// Read the names section into said buffer.
	sh_name_tmp[name_sh->file_size] = 0;
	SEEK(name_sh->offset);
	READ(sh_name_tmp, name_sh->file_size);
	
	// Collect section names.
	for (size_t i = 0; i < ctx.num_sect_header; i++) {
		// Determine maximum size.
		size_t maximum = name_sh->file_size - ctx.sect_header[i].name_index;
		// Copy the string from the buffer.
		ctx.sect_header[i].name = strndup(&sh_name_tmp[ctx.sect_header[i].name_index], maximum);
	}
	
	#ifdef ENABLE_DEBUG_LOGS
	// Print info about sections.
	for (size_t i = 0; i < ctx.num_sect_header; i++) {
		printf("Sect header %zu:\n", i);
		const char *name_str = ctx.sect_header[i].name ? ctx.sect_header[i].name : "name not found";
		printf("Name:       %d (%s)\n", ctx.sect_header[i].name_index, name_str);
		printf("Type:       %02x\n", ctx.sect_header[i].type);
		printf("Flags:      %08x\n", ctx.sect_header[i].flags);
		printf("Virt addr:  %08x\n", ctx.sect_header[i].vaddr);
		printf("File offs:  %d\n",   ctx.sect_header[i].offset);
		printf("File size:  %d\n",   ctx.sect_header[i].file_size);
		printf("Link:       %d\n",   ctx.sect_header[i].link);
		printf("Info:       %08x\n", ctx.sect_header[i].info);
		printf("Alignment:  %08x\n", ctx.sect_header[i].alignment);
		printf("Entry size: %08x\n", ctx.sect_header[i].entry_size);
		printf("\n");
	}
	#endif
	
	// Load symbols from symtab.
	elf_sh_t *symtab = elf_find_sect(&ctx, ".symtab");
	#ifdef ENABLE_DEBUG_LOGS
	if (symtab) {
		printf("ELF contains .symtab\n\n");
	} else {
		printf("ELF does not contain .symtab\n\n");
	}
	#endif
	if (symtab && needs_extra_info) {
		
		// Validate symtab type.
		if (symtab->type != 0x02 || symtab->entry_size < 0x10) goto error;
		size_t n_sym = symtab->file_size / symtab->entry_size;
		if (n_sym * symtab->entry_size != symtab->file_size) goto error;
		
		// Find strings table section.
		elf_sh_t *strtab = &ctx.sect_header[symtab->link];
		if (strtab->type != 0x03 || strtab->file_size < 3) goto error;
		
		// Cache .strtab contents.
		name_tmp = malloc(strtab->file_size + 1);
		if (!name_tmp) goto error;
		name_tmp[strtab->file_size] = 0;
		SEEK(strtab->offset);
		READ(name_tmp, strtab->file_size);
		
		// Allocate memory for symbols.
		ctx.symbols = malloc(sizeof(elf_sym_t) * n_sym);
		if (!ctx.symbols) goto error;
		memset(ctx.symbols, 0, sizeof(elf_sym_t) * n_sym);
		ctx.num_symbols = n_sym;
		
		// Read section contents.
		for (size_t i = 0; i < n_sym; i++) {
			// Read raw symbol data.
			SEEK(symtab->offset + i * symtab->entry_size);
			READUINT(ctx.symbols[i].name_index, 4);
			READUINT(ctx.symbols[i].value, 4);
			READUINT(ctx.symbols[i].size, 4);
			READUINT(ctx.symbols[i].info, 1);
			SKIP(1);
			READUINT(ctx.symbols[i].sect_idx, 2);
			if (ctx.symbols[i].sect_idx >= ctx.num_sect_header) {
				ctx.symbols[i].sect_idx = 0;
			}
			ctx.symbols[i].type    = ctx.symbols[i].info & 0x0f;
			ctx.symbols[i].binding = ctx.symbols[i].info >> 4;
			
			if (ctx.symbols[i].name_index) {
				// Determine maximum name length.
				size_t maximum = strtab->file_size - ctx.symbols[i].name_index - 1;
				// Grab a copy.
				ctx.symbols[i].name = strndup(&name_tmp[ctx.symbols[i].name_index], maximum);
			}
			
			#ifdef ENABLE_DEBUG_LOGS
			// Print out some stuff.
			printf("Symbol %d (%02x):\n", i, ctx.symbols[i].type);
			if (ctx.symbols[i].name) printf("Name:    %d (%s)\n", ctx.symbols[i].name_index, ctx.symbols[i].name);
			else printf("Name:    %d (name not found)\n", ctx.symbols[i].name_index);
			printf("Value:   %08x\n", ctx.symbols[i].value);
			printf("Size:    %d\n",   ctx.symbols[i].size);
			printf("Info:    %02x\n", ctx.symbols[i].info);
			printf("Section: %d\n",   ctx.symbols[i].sect_idx);
			printf("\n");
			#endif
		}
		
	}
	
	// Clean up the names buffer.
	free(sh_name_tmp);
	sh_name_tmp = NULL;
	free(name_tmp);
	name_tmp = NULL;
	
	// Done interpreting.
	ctx.valid = true;
	return ctx;
	
	// Clean up on error.
	error:
	if (ctx.prog_header) free(ctx.prog_header);
	if (ctx.sect_header) {
		for (size_t i = 0; i < ctx.num_sect_header; i++) {
			if (ctx.sect_header[i].name) free(ctx.sect_header[i].name);
		}
		free(ctx.sect_header);
	}
	if (ctx.symbols) {
		for (size_t i = 0; i < ctx.num_symbols; i++) {
			if (ctx.symbols[i].name) free(ctx.symbols[i].name);
		}
		free(ctx.symbols);
	}
	if (name_tmp) free(name_tmp);
	return (elf_ctx_t) { false };
}

// Release resources from a loaded `elf_ctx_t`.
void elf_free_ctx(elf_ctx_t ctx) {
	if (ctx.prog_header) free(ctx.prog_header);
	if (ctx.sect_header) {
		for (size_t i = 0; i < ctx.num_sect_header; i++) {
			if (ctx.sect_header[i].name) free(ctx.sect_header[i].name);
		}
		free(ctx.sect_header);
	}
	if (ctx.symbols) {
		for (size_t i = 0; i < ctx.num_symbols; i++) {
			if (ctx.symbols[i].name) free(ctx.symbols[i].name);
		}
		free(ctx.symbols);
	}
}


// Load an ELF file into preallocated memory.
elf_loaded_t elf_load(FILE *fd, elf_ctx_t *ctx, size_t paddr, size_t cap) {
	if (!ctx || !fd || !ctx->valid) return (elf_loaded_t) { false };
	
	elf_loaded_t loaded;
	loaded.ctx = ctx;
	loaded.memory = NULL;
	
	bool is_little_endian = ctx->is_le;
	
	// Assert file is relocatable.
	if (ctx->type != ET_EXEC) {
		printf("Error: Cannot load ELF type %02x (supported: 0x02; executable)\n", ctx->type);
		goto error;
	}
	
	// Assert machine type.
	if (ctx->machine != 0x28) {
		printf("Error: Cannot load ELF for machine type %02x (supported: 0x28; ARM)\n", ctx->machine);
		goto error;
	}
	
	// Determine required size.
	size_t min_addr = SIZE_MAX;
	size_t max_addr = 0;
	for (size_t i = 0; i < ctx->num_prog_header; i++) {
		elf_ph_t *ph = &ctx->prog_header[i];
		if (ph->type == 1) {
			if (ph->vaddr < min_addr) {
				min_addr = ph->vaddr;
			}
			if (ph->vaddr + ph->mem_size > max_addr) {
				max_addr = ph->vaddr + ph->mem_size;
			}
		}
	}
	size_t required = max_addr - min_addr;
	if (min_addr == SIZE_MAX) return (elf_loaded_t) { false };
	
	// Check memory requirement.
	if (required > cap) {
		printf("Error: Cannot load ELF file of size %zu; capacity is %zu\n", required, cap);
		goto error;
	}
	if (min_addr < paddr) {
		printf("Error: Cannot satisfy address of ELF file: minimum address request is %08zx; available is %08zx", min_addr, paddr);
		goto error;
	}
	if (max_addr > paddr + cap) {
		printf("Error: Cannot satisfy address of ELF file: maximum address request is %08zx; available is %08zx", min_addr - 1, paddr + cap - 1);
		goto error;
	}
	
	// Zero out memory where needed.
	for (size_t i = 0; i < ctx->num_prog_header; i++) {
		elf_ph_t *ph = &ctx->prog_header[i];
		if (ph->type == 1) {
			// Load this straight into memory.
			uint32_t space = ph->mem_size - ph->file_size;
			memset((void *) (ph->vaddr + ph->mem_size - space), 0, space);
		}
	}
	
	// Load data into memory.
	for (size_t i = 0; i < ctx->num_prog_header; i++) {
		elf_ph_t *ph = &ctx->prog_header[i];
		if (ph->type == 1) {
			// Load this straight into memory.
			fseek(fd, ph->offset, SEEK_SET);
			size_t read = fread((void *) ph->vaddr, 1, ph->file_size, fd);
			
			// Double check load success.
			if (read < ph->file_size) {
				printf("Error: Unexpected end of file\n");
				goto error;
			}
		}
	}
	
	loaded.vaddr        = min_addr;
	loaded.memory       = (void *) min_addr;
	loaded.mem_size     = required;
	loaded.num_sections = 0;
	loaded.sections     = 0;
	loaded.num_symbols  = 0;
	loaded.symbols      = 0;
	loaded.valid        = true;
	return loaded;
	
	error:
	return (elf_loaded_t) { false };
}


// Get the section header with the specified name.
elf_sh_t *elf_find_sect(elf_ctx_t *ctx, const char *name) {
	for (size_t i = 0; i < ctx->num_sect_header; i++) {
		if (ctx->sect_header[i].name && !strcmp(name, ctx->sect_header[i].name)) {
			return &ctx->sect_header[i];
		}
	}
	return NULL;
}

// Find a symbol from the table.
elf_sym_t *elf_find_sym(elf_ctx_t *ctx, const char *name) {
	for (size_t i = 0; i < ctx->num_symbols; i++) {
		if (ctx->symbols[i].name && !strcmp(ctx->symbols[i].name, name)) {
			return &ctx->symbols[i];
		}
	}
	return NULL;
}

// Get the loaded address of a symbol.
// Returns NULL when not found.
void *elf_adrof_sym(elf_loaded_t *loaded, const char *name, bool allow_object, bool allow_function) {
	elf_sym_t *sym = elf_find_sym(loaded->ctx, name);
	if (sym && (sym->info & 0xf0) == 0x10 && (((sym->info & 0x0f) == 0x01 && allow_object) || ((sym->info & 0x0f) == 0x02 && allow_function))) {
		return (void *) ((size_t) sym->value + loaded->vaddr);
	} else {
		return NULL;
	}
}
