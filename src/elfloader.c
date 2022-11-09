
#include "elfloader.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>



// Does nothing.
#define R_ARM_NONE		0
// Deprecated:			((S + A) | T) - P
#define R_ARM_PC24		1
// Static, Data:		(S + A) | T
#define R_ARM_ABS32		2
// Static, Data:		((S + A) | T) | -P
#define R_ARM_REL32 	3
// Static, Arm:			S + A - P
#define R_ARM_LDR_PC_G0	4
// Static, Data:		S + A
#define R_ARM_ABS16		5
// Static, Arm:			S + A
#define R_ARM_ABS12		6
// Static, Thumb16:		S + A
#define R_ARM_THM_ABS5	7
// Static, Data:		S + A
#define R_ARM_ABS8		8
// Static, Data:		((S + A) | T) - B(S)
#define R_ARM_SBREL32	9
// Static, Thumb32:		((S + A) | T) - P
#define R_ARM_THM_CALL	10
// Static, Thumb16:		

/*
1 R_ARM_PC24 Deprecated Arm ((S + A) | T) – P
2 R_ARM_ABS32 Static Data (S + A) | T
3 R_ARM_REL32 Static Data ((S + A) | T) | – P
4 R_ARM_LDR_PC_G0 Static Arm S + A – P
5 R_ARM_ABS16 Static Data S + A
6 R_ARM_ABS12 Static Arm S + A
7 R_ARM_THM_ABS5 Static Thumb16 S + A
8 R_ARM_ABS8 Static Data S + A
9 R_ARM_SBREL32 Static Data ((S + A) | T) – B(S)
10 R_ARM_THM_CALL Static Thumb32 ((S + A) | T) – P
11 R_ARM_THM_PC8 Static Thumb16 S + A – Pa
12 R_ARM_BREL_ADJ Dynamic Data ChangeIn[B(S)] + A
17 R_ARM_TLS_DTPMOD32 Dynamic Data Module[S]
18 R_ARM_TLS_DTPOFF32 Dynamic Data S + A – TLS
19 R_ARM_TLS_TPOFF32 Dynamic Data S + A – tp
21 R_ARM_GLOB_DAT Dynamic Data (S + A) | T
22 R_ARM_JUMP_SLOT Dynamic Data (S + A) | T
23 R_ARM_RELATIVE Dynamic Data B(S) + A [Note: see Dynamic relocations]
24 R_ARM_GOTOFF32 Static Data ((S + A) | T) – GOT_ORG
25 R_ARM_BASE_PREL Static Data B(S) + A – P
26 R_ARM_GOT_BREL Static Data GOT(S) + A – GOT_ORG
27 R_ARM_PLT32 Deprecated Arm ((S + A) | T) – P
28 R_ARM_CALL Static Arm ((S + A) | T) – P
29 R_ARM_JUMP24 Static Arm ((S + A) | T) – P
30 R_ARM_THM_JUMP24 Static Thumb32 ((S + A) | T) – P
31 R_ARM_BASE_ABS Static Data B(S) + A
35 R_ARM_LDR_SBREL_11_0_NC Deprecated Arm S + A – B(S)
36 R_ARM_ALU_SBREL_19_12_NC Deprecated Arm S + A – B(S)
37 R_ARM_ALU_SBREL_27_20_CK Deprecated Arm S + A – B(S)
38 R_ARM_TARGET1 Static Miscellaneous (S + A) | T or ((S + | A) | T) – P
39 R_ARM_SBREL31 Deprecated Data ((S + A) | T) – B(S)
42 R_ARM_PREL31 Static Data ((S + A) | T) – P
43 R_ARM_MOVW_ABS_NC Static Arm (S + A) | T
44 R_ARM_MOVT_ABS Static Arm S + A
45 R_ARM_MOVW_PREL_NC Static Arm ((S + A) | T) – P
46 R_ARM_MOVT_PREL Static Arm S + A – P
47 R_ARM_THM_MOVW_ABS_NC Static Thumb32 (S + A) | T
48 R_ARM_THM_MOVT_ABS Static Thumb32 S + A
49 R_ARM_THM_MOVW_PREL_NC Static Thumb32 ((S + A) | T) – P
50 R_ARM_THM_MOVT_PREL Static Thumb32 S + A – P
51 R_ARM_THM_JUMP19 Static Thumb32 ((S + A) | T) – P
52 R_ARM_THM_JUMP6 Static Thumb16 S + A – P
53 R_ARM_THM_ALU_PREL_11_0 Static Thumb32 ((S + A) | T) – Pa
54 R_ARM_THM_PC12 Static Thumb32 S + A – Pa
55 R_ARM_ABS32_NOI Static Data S + A
56 R_ARM_REL32_NOI Static Data S + A – P
57 R_ARM_ALU_PC_G0_NC Static Arm ((S + A) | T) – P
58 R_ARM_ALU_PC_G0 Static Arm ((S + A) | T) – P
59 R_ARM_ALU_PC_G1_NC Static Arm ((S + A) | T) – P
60 R_ARM_ALU_PC_G1 Static Arm ((S + A) | T) – P
61 R_ARM_ALU_PC_G2 Static Arm ((S + A) | T) – P
62 R_ARM_LDR_PC_G1 Static Arm S + A – P
63 R_ARM_LDR_PC_G2 Static Arm S + A – P
64 R_ARM_LDRS_PC_G0 Static Arm S + A – P
65 R_ARM_LDRS_PC_G1 Static Arm S + A – P
66 R_ARM_LDRS_PC_G2 Static Arm S + A – P
67 R_ARM_LDC_PC_G0 Static Arm S + A – P
68 R_ARM_LDC_PC_G1 Static Arm S + A – P
69 R_ARM_LDC_PC_G2 Static Arm S + A – P
70 R_ARM_ALU_SB_G0_NC Static Arm ((S + A) | T) – B(S)
71 R_ARM_ALU_SB_G0 Static Arm ((S + A) | T) – B(S)
72 R_ARM_ALU_SB_G1_NC Static Arm ((S + A) | T) – B(S)
73 R_ARM_ALU_SB_G1 Static Arm ((S + A) | T) – B(S)
74 R_ARM_ALU_SB_G2 Static Arm ((S + A) | T) – B(S)
75 R_ARM_LDR_SB_G0 Static Arm S + A – B(S)
76 R_ARM_LDR_SB_G1 Static Arm S + A – B(S)
77 R_ARM_LDR_SB_G2 Static Arm S + A – B(S)
78 R_ARM_LDRS_SB_G0 Static Arm S + A – B(S)
79 R_ARM_LDRS_SB_G1 Static Arm S + A – B(S)
80 R_ARM_LDRS_SB_G2 Static Arm S + A – B(S)
81 R_ARM_LDC_SB_G0 Static Arm S + A – B(S)
82 R_ARM_LDC_SB_G1 Static Arm S + A – B(S)
83 R_ARM_LDC_SB_G2 Static Arm S + A – B(S)
84 R_ARM_MOVW_BREL_NC Static Arm ((S + A) | T) – B(S)
85 R_ARM_MOVT_BREL Static Arm S + A – B(S)
86 R_ARM_MOVW_BREL Static Arm ((S + A) | T) – B(S)
87 R_ARM_THM_MOVW_BREL_NC Static Thumb32 ((S + A) | T) – B(S)
88 R_ARM_THM_MOVT_BREL Static Thumb32 S + A – B(S)
89 R_ARM_THM_MOVW_BREL Static Thumb32 ((S + A) | T) – B(S)
94 R_ARM_PLT32_ABS Static Data PLT(S) + A
95 R_ARM_GOT_ABS Static Data GOT(S) + A
96 R_ARM_GOT_PREL Static Data GOT(S) + A – P
97 R_ARM_GOT_BREL12 Static Arm GOT(S) + A – GOT_ORG
98 R_ARM_GOTOFF12 Static Arm S + A – GOT_ORG
102 R_ARM_THM_JUMP11 Static Thumb16 S + A – P
103 R_ARM_THM_JUMP8 Static Thumb16 S + A – P
104 R_ARM_TLS_GD32 Static Data GOT(S) + A – P
105 R_ARM_TLS_LDM32 Static Data GOT(S) + A – P
106 R_ARM_TLS_LDO32 Static Data S + A – TLS
107 R_ARM_TLS_IE32 Static Data GOT(S) + A – P
108 R_ARM_TLS_LE32 Static Data S + A – tp
109 R_ARM_TLS_LDO12 Static Arm S + A – TLS
110 R_ARM_TLS_LE12 Static Arm S + A – tp
111 R_ARM_TLS_IE12GP Static Arm GOT(S) + A – GOT_ORG
131 R_ARM_THM_GOT_BREL12 Static Thumb32 GOT(S) + A – GOT_ORG
132 R_ARM_THM_ALU_ABS_G0_NC Static Thumb16 (S + A) | T
133 R_ARM_THM_ALU_ABS_G1_NC Static Thumb16 S + A
134 R_ARM_THM_ALU_ABS_G2_NC Static Thumb16 S + A
135 R_ARM_THM_ALU_ABS_G3 Static Thumb16 S + A
136 R_ARM_THM_BF16 Static Arm ((S + A) | T) – P
137 R_ARM_THM_BF12 Static Arm ((S + A) | T) – P
138 R_ARM_THM_BF18 Static Arm ((S + A) | T) – P
*/


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


const uint8_t elf_magic[4] = { 0x7f, 'E', 'L', 'F' };



// Load an ELF file into memory.
elf_ctx_t elf_interpret(FILE *fd) {
	// Check magic.
	EXPECT(4, elf_magic);
	elf_ctx_t ctx;
	ctx.prog_header     = NULL;
	ctx.num_prog_header = 0;
	ctx.sect_header     = NULL;
	ctx.num_sect_header = 0;
	ctx.symbols         = NULL;
	ctx.num_symbols     = 0;
	
	// Check 32 bit type.
	EXPECT(1, (const char[1]){1});
	
	// Read endianness.
	int is_little_endian = true;
	READUINT(is_little_endian, 1);
	is_little_endian = is_little_endian == 1;
	ctx.is_le = is_little_endian;
	uint8_t version;
	READUINT(version, 1);
	printf("ELF V%d %s endian\n", version, is_little_endian ? "little" : "big");
	
	// Check target type.
	uint8_t osabi, abiver;
	READUINT(osabi,  1);
	READUINT(abiver, 1);
	printf("OSABI %02x v%d\n", osabi, abiver);
	
	// Skip padding.
	SKIP(7);
	
	// Check machine type.
	uint16_t type, machine;
	READUINT(type,    2);
	READUINT(machine, 2);
	printf("ELF type %04x\n", type);
	printf("ISA type %02x\n", machine);
	ctx.type    = type;
	ctx.machine = machine;
	
	// There is another verion???
	SKIP(4);
	
	// Offsets.
	uint32_t prog_header_offs, sect_header_offs;
	READUINT(ctx.entrypoint, 4);
	printf("Entrypoint %08x\n", ctx.entrypoint);
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
	printf("%u prog headers (%u each, starting at %08x)\n", ctx.num_prog_header, ph_ent_size, prog_header_offs);
	
	// Section header.
	uint32_t sh_ent_size, sh_name_idx;
	READUINT(sh_ent_size,         2);
	READUINT(ctx.num_sect_header, 2);
	READUINT(sh_name_idx,         2);
	printf("%u sect headers (%u each, starting at %08x)\n", ctx.num_sect_header, sh_ent_size, sect_header_offs);
	printf("Name section index %d\n", sh_name_idx);
	printf("\n");
	
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
	char *name_tmp = malloc(name_sh->file_size + 1);
	if (!name_tmp) goto error;
	// Read the names section into said buffer.
	name_tmp[name_sh->file_size] = 0;
	SEEK(name_sh->offset);
	READ(name_tmp, name_sh->file_size);
	
	// Collect section names.
	for (size_t i = 0; i < ctx.num_sect_header; i++) {
		// Determine maximum size.
		size_t maximum = name_sh->file_size - ctx.sect_header[i].name_index;
		// Copy the string from the buffer.
		ctx.sect_header[i].name = strndup(&name_tmp[ctx.sect_header[i].name_index], maximum);
	}
	// Clean up the names buffer.
	free(name_tmp);
	name_tmp = NULL;
	
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
	
	// Load symbols from symtab.
	elf_sh_t *symtab = elf_find_sect(&ctx, ".symtab");
	if (symtab) {
		printf("ELF contains .symtab\n\n");
		
		// Validate symtab type.
		if (symtab->type != 0x02 || symtab->entry_size < 0x10) goto error;
		size_t n_sym = symtab->file_size / symtab->entry_size;
		if (n_sym * symtab->entry_size != symtab->file_size) goto error;
		
		// Find strings table section.
		elf_sh_t *strtab = elf_find_sect(&ctx, ".strtab");
		if (!strtab) goto error;
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
			
			if (ctx.symbols[i].name_index) {
				// Determine maximum name length.
				size_t maximum = strtab->file_size - ctx.symbols[i].name_index - 1;
				// Grab a copy.
				ctx.symbols[i].name = strndup(&name_tmp[ctx.symbols[i].name_index], maximum);
			}
			
			// Print out some stuff.
			if (ctx.symbols[i].name) printf("Symbol %d (%s):\n", i, ctx.symbols[i].name);
			else printf("Symbol %d (name not found):\n", i);
			printf("Value:   %08x\n", ctx.symbols[i].value);
			printf("Size:    %d\n",   ctx.symbols[i].size);
			printf("Info:    %02x\n", ctx.symbols[i].info);
			printf("Section: %d\n",   ctx.symbols[i].sect_idx);
			printf("\n");
		}
		
	} else {
		// There are no symbols.
		printf("ELF is stripped.\n\n");
	}
	
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

// Load an ELF file for execution.
elf_loaded_t elf_load(FILE *fd, elf_ctx_t *ctx) {
	if (!ctx || !fd || !ctx->valid) return (elf_loaded_t) { false };
	
	elf_loaded_t loaded;
	loaded.ctx = ctx;
	loaded.memory = NULL;
	
	bool is_little_endian = ctx->is_le;
	
	// Assert file is relocatable.
	if (ctx->type != ET_REL) {
		printf("Error: Cannot load ELF type %02x (supported: 0x02; executable)\n", ctx->type);
		goto error;
	}
	printf("Warning: ELF files loaded by the simple method are very restricted and might cause crashes.\n");
	
	// Assert machine type.
	if (ctx->machine != 0x28) {
		printf("Error: Cannot load ELF for machine type %02x (supported: 0x28; ARM)\n", ctx->machine);
		goto error;
	}
	
	// TODO: Determine alignment.
	size_t alignment = 256;
	
	// Minumum address (inclusive)
	size_t min_addr = SIZE_MAX;
	// Maximum address (exclusive)
	size_t max_addr = 0;
	
	// Determine required size.
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
	
	// Allocate memory.
	loaded.memory = aligned_alloc(required, alignment);
	if (!loaded.memory) return (elf_loaded_t) { false };
	loaded.vaddr  = (size_t) loaded.memory - min_addr;
	
	// Load segments into memory.
	for (size_t i = 0; i < ctx->num_prog_header; i++) {
		elf_ph_t *ph = &ctx->prog_header[i];
		if (ph->type == 1) {
			if (ph->mem_size < ph->file_size) goto error;
			void *offs = (void *) (loaded.vaddr + ph->vaddr);
			SEEK(ph->offset);
			READ(offs, ph->file_size);
			offs = (void *) (loaded.vaddr + ph->vaddr + ph->file_size);
			memset(offs, 0, ph->mem_size - ph->file_size);
		}
	}
	
	// Look for a global offset table.
	elf_sh_t *got = elf_find_sect(ctx, ".got");
	if (got) {
		// Verify the section's validity.
		if (got->entry_size != 4 || got->file_size & 3) goto error;
		
		// Apply offsets to global offset table.
		uint32_t *gotptr = (uint32_t *) (got->vaddr + loaded.vaddr);
		SEEK(got->offset);
		for (size_t i = 0; i < got->file_size / 4; i ++) {
			READUINT(gotptr[i], 4);
			gotptr[i] += loaded.vaddr;
		}
	}
	
	loaded.valid = true;
	return loaded;
	
	error:
	if (loaded.memory) aligned_free(loaded.memory);
	return (elf_loaded_t) { false };
}

// Advanced loading method.
elf_link_t elf_linked_load(size_t num_to_load, elf_ctx_t **to_load, FILE **to_load_fds, size_t num_loaded, elf_loaded_t **loaded) {
	elf_link_t linked;
	linked.num_loaded = 0;
	linked.loaded = NULL;
	
	// Interpret ELF files that weren't already.
	for (size_t i = 0; i < num_to_load; i++) {
		if (!to_load[i]->valid) {
			*to_load[i] = elf_interpret(to_load_fds[i]);
		}
	}
	
	// Ensure validity of all inputs.
	for (size_t i = 0; i < num_to_load; i++) {
		if (!to_load[i]->valid) goto error;
	}
	for (size_t i = 0; i < num_loaded; i++) {
		if (!loaded[i]->valid) goto error;
	}
	
	// TODO: Enforce no duplicate symbols.
	
	// Clean up on error.
	error:
	if (linked.loaded) {
		free(linked.loaded);
	}
	return (elf_link_t) { false };
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
