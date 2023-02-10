SECTIONS
{
	/* Origin is different than usual; near the end of RAM */
	. = 0x20030000;
	
	/*
		Table of pointers that need to be filled in.
		The first index points to the entry vector.
		The middle is a list of string pointers that are to be replaced.
		Null-terminated.
	*/
	.ptrtab : { *(.ptrtab) }
	
	/* Create symbols to keep track of `.init_array`. */
	.init_array : {
		__start_init_array = .;
		*(.init_array)
		__stop_init_array = .;
	}
	
	/* The usual ELF sections */
	.text   : { *(.text) *(.text*) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data   : { *(.data) *(.data*) }
	.bss    : { *(.bss) *(.bss*) }
}

/*
	Set the entry vector to _start.
	Note that in this ABI, `int _start(void)` is a function that returns the program's exit code.
	It is intended to be a wrapper around `int main(void)` to perform basic set-up.
*/
ENTRY(_start)
