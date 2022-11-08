#include <stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "elfloader.h"

#define MPU_TYPE (*(uint32_t *) (PPB_BASE + 0xed90))
#define MPU_CTRL (*(uint32_t *) (PPB_BASE + 0xed94))
#define MPU_RNR  (*(uint32_t *) (PPB_BASE + 0xed98))
#define MPU_RBAR (*(uint32_t *) (PPB_BASE + 0xed9c))
#define MPU_RASR (*(uint32_t *) (PPB_BASE + 0xeda0))

extern const unsigned char elf_file[];
extern const unsigned int elf_file_length;

int main() {
	stdio_init_all();
	sleep_ms(2500);
	printf("\n\n\n\n\n\n\n\n\n\n\n\nStartup time!\n\n");
	FILE *elf_fd = fmemopen((void *) elf_file, elf_file_length, "r");
	
	elf_ctx_t ctx = elf_interpret(elf_fd);
	if (ctx.valid) {
		printf("Interpret success!\n");
		sleep_ms(2000);
		elf_loaded_t loaded = elf_load(elf_fd, &ctx);
		if (loaded.valid) {
			printf("Loaded at %08zx\n", loaded.vaddr);
			
			elf_sym_t *fancy_sym = elf_find_sym(&ctx, "reset");
			if (fancy_sym) {
				size_t addr = fancy_sym->value + loaded.vaddr;
				printf("Sym reset @ %08zx\n", addr);
				for (size_t i = 0; i < 32; i += 8) {
					for (size_t x = 0; x < 8; x++) {
						printf(" %02x", ((uint8_t *) addr)[i+x]);
					}
					printf("\n");
				}
				
				sleep_ms(2000);
				while (1) sleep_ms(100);
				// int (*fptr)();
				// fptr = (void *) addr;
				// int retval = fptr();
				// printf("Returns %08x\n", retval);
				
			} else {
				printf("Sym reset not found\n");
			}
		} else {
			printf("Load error!\n");
		}
	} else {
		printf("Interpret error!\n");
	}
	fflush(stdout);
	
	while(1) sleep_ms(100);
}
