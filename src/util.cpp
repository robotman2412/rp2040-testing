
#include "util.h"
#include <stdio.h>

static char hexdumpFilter(char in) {
	if (in < 0x20 || in >= 0x7f) {
		return '.';
	} else {
		return in;
	}
}

extern "C" void hexdump(const void *_memory, size_t len, size_t cols) {
	auto memory = (const uint8_t *) _memory;
	
	// Determine amount of rows to print.
	size_t rows  = len / cols;
	size_t extra = len % cols;
	
	// Print full rows.
	size_t row;
	for (size_t row = 0; row < rows; row++) {
		// Hexadecimal.
		for (size_t col = 0; col < cols; col++) {
			printf("%02x ", memory[row*cols+col]);
		}
		// ASCII.
		for (size_t col = 0; col < cols; col++) {
			putchar(hexdumpFilter(memory[row*cols+col]));
		}
		// The newline.
		putchar('\n');
	}
	
	// Print the last, partial row.
	if (extra) {
		// Hexadecimal.
		size_t col;
		for (col = 0; col < extra; col++) {
			printf("%02x ", memory[row*cols+col]);
		}
		// Padding.
		for (; col < cols; col++) {
			printf("   ");
		}
		// ASCII.
		for (col = 0; col < cols; col++) {
			putchar(hexdumpFilter(memory[row*cols+col]));
		}
		// The newline.
		putchar('\n');
	}
}
