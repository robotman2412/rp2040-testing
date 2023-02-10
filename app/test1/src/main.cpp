
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

#include <stdio.h>

struct dumdum {
	int a;
	dumdum() {
		printf("dumdum()\n");
		a = 3;
	}
	~dumdum() {
		printf("~dumdum()\n");
	}
};

static dumdum mine;

int quantum = 123;

extern "C" int main() {
	printf("quantum %d\n", quantum);
	return 0;
}
