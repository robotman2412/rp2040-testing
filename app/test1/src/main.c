
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

static elf_client_data_t _client_data;

static int _pre_resolve_callback(int in);

static int (*_pointer_callback)(int) = _pre_resolve_callback;

static int _pre_resolve_callback(int in) {
	_pointer_callback = _client_data.resolver(_client_data.resolver_ctx, "callback", false, true);
	if (!_pointer_callback) _client_data.abort();
	return _pointer_callback(in);
}

int callback(int in) {
	return _pointer_callback(in);
}

static void dummy() {}

#define PREINITDATA dummy
void (*preinitvar)() = PREINITDATA;

static void *ptr_array[] = {
	dummy, dummy, dummy, dummy,
};

void _start(elf_client_data_t client_data) {
	static void *preinitlocal = PREINITDATA;
	_client_data = client_data;
	_pointer_callback = _pre_resolve_callback;
	callback((int) preinitvar);
	preinitvar = PREINITDATA;
	callback((int) preinitvar);
	callback((int) preinitlocal);
}
