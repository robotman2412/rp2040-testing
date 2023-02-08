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
	
	/* The usual ELF sections */
	.text   : { *(.text) }
	.rodata : { *(.rodata) }
	.data   : { *(.data) }
	.bss    : { *(.bss) }
}