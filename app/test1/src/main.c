
// R0-R12:	General regs
// MSP:		Main stack pointer
// PSP:		Process stack pointer
// LR:		Link register; used for returns
// PC:		Program counter; PC[0] is stored into EPSR T-bit
// PSR:		Program status register; bitwise OR of APSR, IPSR, EPSR
// APSR:		Application Program Status Register; [31]: negative; [30]: zero; [29]: carry; [28]: overflow
// IPSR:		Interrupt Program Status Register
// EPSR:		Execution Program Status Register; [24]: Thumb state (which IS to run)
// PRIMASK:	Priority mask register
// CONTROL:	Control register; [1]: stack sel (0: MSP as stack, 1: PSP as stack)

// Calling conventions:
// R0-R3:	Arguments / return values
// R4-R11:	Locals (callee saved)

#include <elfloader_client_data.h>

int lol = 0xdeadbeef;
// static int lmao = 0x12345678;

// int quantum() {
// 	return lol;
// }

// int dot() {
// 	return lmao;
// }

// void _start(elf_client_data_t client_data) {
	
// }
