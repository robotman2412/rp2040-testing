#include <stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "elfloader.h"
#include "abi_impl.h"
#include "ili9341.h"
#include <hardware/spi.h>
#include "pax_gfx.h"

extern const unsigned char elf_file[];
extern const unsigned int elf_file_length;

#define SPI_MOSI    19
#define SPI_MISO    16 // (unused)
#define SPI_SCLK    18

#define DISP_RST 13
#define DISP_CS  14
#define DISP_DCX 15 // (same as SPI_DC for display)

bool match_ptrtab(void **entry) {
	if ((size_t) *entry < 0x20030000 || (size_t) *entry > 0x2003efff) {
		printf("Invalid entry in ptrtab (%p).\n", *entry);
		return false;
	}
	
	for (size_t i = 0; i < abi_lut_len; i++) {
		if (!strcmp(*entry, abi_lut[i].name)) {
			*entry = abi_lut[i].pointer;
			return true;
		}
	}
	
	printf("%s not found.\n", (const char *) *entry);
	return false;
}

int unhexc(char in) {
	if (in >= '0' && in <= '9') {
		return in - '0';
	} else if (in >= 'a' && in <= 'f') {
		return in - 'a' + 0xa;
	} else if (in >= 'A' && in <= 'F') {
		return in - 'A' + 0xa;
	}
	return -1;
}

FILE *input_test() {
	uint8_t *tmp = malloc(80*1024);
	printf("Waiting for datas.\n");
	size_t index = 0;
	
	while (1) {
		int c;
		while((c = getchar()) <= 0x20);
		if (c == '!') {
			putchar('\n');
			break;
		}
		putchar(c);
		
		uint8_t decd = 0;
		int msb = unhexc(c);
		while((c = getchar()) == EOF);
		putchar(c);
		int lsb = unhexc(c);
		if (msb >= 0 && lsb >= 0) {
			decd = (msb<<4) | lsb;
		}
		
		tmp[index] = decd;
		index++;
	}
	
	printf("Got datas:\n");
	for (size_t i = 0; i < index; i++) {
		printf("%02x ", tmp[i]);
	}
	printf("\n");
	sleep_ms(50);
	
	return fmemopen(tmp, index, "r");
}

int main() {
	stdio_init_all();
	sleep_ms(2500);
	printf("\n\n\n\n\n\n\n\n\n\n\n\nStartup time!\n\n");
	sleep_ms(500);
	
	size_t fb_len = 320 * 240;
	size_t fb_size = 2 * fb_len;
	uint16_t *fb = malloc(fb_size);
	pax_buf_t buf;
	pax_buf_init(&buf, fb, 320, 240, PAX_BUF_16_565RGB);
	pax_buf_reversed(&buf, true);
	memset(fb, 0, fb_size);
	
	uint speed = spi_init(spi0, 40000000);
	spi_set_format(spi0, 8, 0, 0, SPI_MSB_FIRST);
	gpio_set_function(SPI_MISO, GPIO_FUNC_SPI);
	gpio_set_function(SPI_MOSI, GPIO_FUNC_SPI);
	gpio_set_function(SPI_SCLK, GPIO_FUNC_SPI);
	gpio_init(DISP_CS);
	gpio_init(DISP_DCX);
	gpio_init(DISP_RST);
	printf("SPI speed is %u\n", speed);
	
	ILI9341 disp = {
		.spi_bus    = 0,
		.pin_cs     = DISP_CS,
		.pin_dcx    = DISP_DCX,
		.pin_reset  = DISP_RST,
		.rotation   = 1,
		.color_mode = true,
		.callback   = NULL,
		.spi_max_transfer_size = 8192,
	};
	ili9341_init(&disp);
	while (1) {
		pax_background(&buf, 0);
		pax_push_2d(&buf);
		pax_apply_2d(&buf, matrix_2d_translate(160, 120));
		pax_apply_2d(&buf, matrix_2d_scale(50, 50));
		uint32_t now = to_ms_since_boot(get_absolute_time());
		float angle = now % 3000 / 3000.0 * M_PI * 2.0;
		pax_apply_2d(&buf, matrix_2d_rotate(angle));
		pax_draw_rect(&buf, 0xffff0000, -1, -1, 2, 2);
		pax_pop_2d(&buf);
		ili9341_write(&disp, fb);
	}
	
	// Interpret ELF file.
	FILE *elf_fd = fmemopen((void *) elf_file, elf_file_length, "r");
	// FILE *elf_fd = input_test();
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
				
				int (*entryFunc)() = (const void *) ctx.entrypoint;
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
	fclose(elf_fd);
	
	while (1);
}
