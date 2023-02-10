#include <stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "elfloader.h"
#include "abi_impl.h"

extern const unsigned char elf_file[];
extern const unsigned int elf_file_length;

int callback(int in) {
	printf("Callback! %d\n", in);
	return in * 2;
}

bool match_ptrtab(void **entry) {
	if ((size_t) *entry < 0x20030000 || (size_t) *entry > 0x2003efff) {
		printf("Invalid entry in ptrtab (%p).\n", *entry)
		return false;
	}
	
	for (size_t i = 0; i < abi_lut_len; i++) {
		if (!strncmp(*entry, abi_lut[i].name)) {
			*entry = abi_lut[i].pointer;
			return true;
		}
	}
	
	printf("%s not found.\n", (const char *) *entry);
	return false;
}

int main() {
	stdio_init_all();
	sleep_ms(2500);
	printf("\n\n\n\n\n\n\n\n\n\n\n\nStartup time!\n\n");
	sleep_ms(500);
	
	// Interpret ELF file.
	FILE *elf_fd = fmemopen((void *) elf_file, elf_file_length, "r");
	elf_ctx_t ctx = elf_interpret(elf_fd, false);
	if (ctx.valid) {
		printf("ELF file valid.\n");
		
		// Load ELF file into dedicated memory.
		elf_loaded_t loaded = elf_load(elf_fd, &ctx, 0x20030000, 0x00010000);
		
		if (loaded.valid) {
			printf("ELF data loaded.\n");
			
			// Obtain some information.
			void **ptrtab = loaded.memory;
			
			// Iterate over ptrtab.
			bool success = true;
			for (size_t i = 0; ptrtab[i]; i++) {
				// Try to resolve this one.
				success &= match_ptrtab(ptrtab + i);
			}
			
			if (success) {
				// Let's try to run it's entry vector.
				printf("Entry %p\n", ctx.entrypoint);
				sleep_ms(50);
				printf("Running.\n");
				
				int (*entryFunc)() = ctx.entrypoint;
				int res = entryFunc();
				printf("Exit code %d (0x%x)\n", res, res);
			} else {
				printf("Pointer table filling failed.\n");
			}
		} else {
			printf("ELF data load failed.\n");
		}
	} else {
		printf("ELF file invalid.\n");
	}
	
	while (1);
}
