
#include "abi_impl.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/reent.h>

uint64_t micros() {
	return get_absolute_time();
}

uint32_t millis() {
	return get_absolute_time() / 1000;
}

void *get_reent_impl() {
	return _REENT;
}

void *get_global_reent_impl() {
	return _GLOBAL_REENT;
}

const abi_entry_t abi_lut[] = {
	// From abi_stdio.h
	
	{ "__get_reent", get_reent_impl },
	{ "__get_global_reent", get_global_reent_impl },
	
	{ "tmpfile", tmpfile },
	{ "tmpnam", tmpnam },
	{ "fopen", fopen },
	{ "fclose", fclose },
	{ "fflush", fflush },
	{ "freopen", freopen },
	{ "setvbuf", setvbuf },
	
	{ "fprintf", fprintf },
	{ "fscanf", fscanf },
	{ "printf", printf },
	{ "scanf", scanf },
	{ "sscanf", sscanf },
	{ "sprintf", sprintf },
	{ "snprintf", snprintf },
	
	{ "fgetc", fgetc },
	{ "fgets", fgets },
	{ "fputc", fputc },
	{ "fputs", fputs },
	{ "getc", getc },
	{ "getchar", getchar },
	{ "gets", gets },
	{ "putc", putc },
	{ "putchar", putchar },
	{ "puts", puts },
	{ "ungetc", ungetc },
	
	{ "fread", fread },
	{ "fwrite", fwrite },
	
	{ "fgetpos", fgetpos },
	{ "fseek", fseek },
	{ "fsetpos", fsetpos },
	
	{ "ftell", ftell },
	{ "rewind", rewind },
	{ "clearerr", clearerr },
	{ "feof", feof },
	{ "ferror", ferror },
	{ "perror", perror },
	
	{ "remove", remove },
	{ "rename", rename },
	
	// From abi_time.h
	
	{ "micros", micros },
	{ "millis", millis },
	{ "sleep_until", sleep_until },
	{ "sleep_us", sleep_us },
	{ "sleep_ms", sleep_ms },
	
	// TODO: From abi_gpio.h
	
	// From abi_malloc.h
	
	{ "malloc", malloc },
	{ "free", free },
	{ "calloc", calloc },
	{ "realloc", realloc },
	
};
const size_t abi_lut_len = sizeof(abi_lut) / sizeof(abi_entry_t);
