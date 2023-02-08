
// R0-R12:	General regs
// MSP:		Main stack pointer
// PSP:		Process stack pointer
// LR:		Link register; used for returns
// PC:		Program counter; PC[0] is stored into EPSR T-bit
// PSR:		Program status register; bitwise OR of APSR, IPSR, EPSR
// APSR:	Application Program Status Register; [31]: negative; [30]: zero; [29]: carry; [28]: overflow
// IPSR:	Interrupt Program Status Register
// EPSR:	Execution Program Status Register; [24]: Thumb state (which IS to run)
// PRIMASK:	Priority mask register
// CONTROL:	Control register; [1]: stack sel (0: MSP as stack, 1: PSP as stack)

// Calling conventions:
// R0-R3:	Arguments / return values
// R4-R11:	Locals (callee saved)

#include <abi_all.h>

// int _start();
// void sleep_ms(unsigned);

// __attribute__((section(".ptrtab")))
// const void *ptrtab[] = {
// 	_start,
// 	"callback",
// 	"printf",
// 	(void *) 0
// };

// int printf(const char *, ...);

// typedef int(*printf_t)(const char*,...);
// #define printf ((printf_t) ptrtab[2])

// extern int test(const char *, ...);
// extern int test();

// int callback(int in) {
// 	// Really just a function pointer forwardening.
// 	return ( (int(*)(int)) ptrtab[1] )(in);
// }

void ultraprint(const char *str) {
	while (*str) {
		putchar(*str);
		sleep_ms(100);
		str++;
	}
}

int _start() {
	// printf("This is very cool\n");
	ultraprint("Hello, World!\n");
	return 0;
}
