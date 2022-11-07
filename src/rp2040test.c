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
		printf("Load success!\n");
	} else {
		printf("Load error!\n");
	}
	
	while(1) sleep_ms(100);
}
