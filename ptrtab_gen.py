#!/usr/bin/env python3

# pip install pyelftools

from sys import argv
from elftools.elf.elffile import *

api_funcs = []

def get_symbols(elf: ELFFile):
	"""Gets all symbols in the elf file"""
	syms = []
	
	for sect in elf.iter_sections():
		if not isinstance(sect, SymbolTableSection): continue
		for symbol in sect.iter_symbols():
			syms.append(symbol)
	
	return syms

def get_undefined_symbols(syms: list):
	"""Gets all undefined symbols from a list of symbols"""
	out = []
	
	for sym in syms:
		if sym["st_info"]["bind"] == "STB_GLOBAL" \
			and sym["st_shndx"] == "SHN_UNDEF" \
			and sym.name in api_funcs:
			out.append(sym)
	
	return out

def generate_string(fd, name: str, data: str):
	"""Generate a string constant in assembly"""
	fd.write("\t.align 2\n")
	fd.write(name + ":\n")
	fd.write("\t.ascii \"" + data + "\\000\"\n\n")

def generate_wrapper(fd, name: str, ptr_offs: int, ptr_base: int):
	"""Generate a function wrapper given a label and a pointer table index"""
	ptr_offs = ptr_offs * 4
	fd.write("\t.global " + name + "\n")
	fd.write("\t.type " + name + ", %function\n")
	fd.write(name + ":\n")
	fd.write("\t@ Preserve registers used.\n")
	fd.write("\tpush {lr}\n")
	fd.write("\tpush {r7}\n")
	fd.write("\t\n")
	fd.write("\t@ Get function address.\n")
	fd.write("\tldr r7, =" + hex(ptr_base + ptr_offs) + "\n")
	fd.write("\tldr r7, [r7]\n")
	fd.write("\tmov r12, r7\n")
	fd.write("\t\n")
	fd.write("\t@ Perform call.\n")
	fd.write("\tpop {r7}\n")
	fd.write("\tblx r12\n")
	fd.write("\t\n")
	fd.write("\t@ Return\n")
	fd.write("\tpop {pc}\n")
	fd.write("\t\n")
	fd.write("\t.size " + name + ", .-" + name + "\n\n\n")

def process_file(elf_in_fd, asm_out_fd, ptr_base: int):
	"""Process an ELF file from beginning to end"""
	# Do some parsing and checks.
	elf = ELFFile(elf_in_fd)
	syms = get_symbols(elf)
	ndef = get_undefined_symbols(syms)
	
	# Write warning label.
	asm_out_fd.write("\n@ Warning: This is a generated file, do not edit it!\n\n")
	
	# Write system information.
	asm_out_fd.write("\t.cpu cortex-m0plus\n")
	asm_out_fd.write("\t.arch armv6s-m\n")
	asm_out_fd.write("\t.syntax unified\n\n")
	
	# Write string constants.
	asm_out_fd.write("\t.section .rodata\n\n")
	for sym in ndef:
		generate_string(asm_out_fd, "__ptrtab_" + sym.name, sym.name)
	
	# Write ptrtab.
	ptrtab_size = 4 * (2 + len(ndef))
	asm_out_fd.write("\n\n\t.section .ptrtab, \"aw\"\n")
	asm_out_fd.write("\t.global __ptrtab\n")
	asm_out_fd.write("\t.type __ptrtab, %object\n")
	asm_out_fd.write("\t.size __ptrtab, " + str(ptrtab_size) + "\n\n")
	asm_out_fd.write("__ptrtab:\n")
	
	# Write ptrtab entries.
	asm_out_fd.write("\t@ ptrtab entries\n")
	for sym in ndef:
		asm_out_fd.write("\t.word __ptrtab_" + sym.name + "\n")
	
	# Write ptrtab terminator.
	asm_out_fd.write("\n\t@ ptrtab terminator.\n")
	asm_out_fd.write("\t.word 0\n\n\n\n")
	
	# Write function wrappers.
	asm_out_fd.write("\t.text\n")
	asm_out_fd.write("\t.align 1\n")
	asm_out_fd.write("\t.code 16\n")
	asm_out_fd.write("\t.thumb_func\n\n")
	for i in range(len(ndef)):
		sym = ndef[i]
		generate_wrapper(asm_out_fd, sym.name, i, ptr_base)
	
	return True

def find_func_names(in_fd):
	"""Simple extractor for function names from abi.txt"""
	out = []
	while (line := in_fd.readline()) != '':
		line = line.strip()
		if line.startswith("#") or line == '': continue
		out.append(line)
	return out

if __name__ == "__main__":
	if len(argv) != 4 and len(argv) != 5:
		print("Usage: ptrtab_gen.py [infile.elf] [outfile.asm] [abi.txt] <ptrtab_base=20030000>")
		exit(1)
	
	abi_fd = open(argv[3], "r")
	api_funcs = find_func_names(abi_fd)
	abi_fd.close()
	
	in_fd = open(argv[1], "rb")
	out_fd = open(argv[2], "w")
	ptr_base = int(argv[4], 16) if len(argv) == 5 else 0x20030000
	
	res = process_file(in_fd, out_fd, ptr_base)
	
	out_fd.flush()
	out_fd.close()
	in_fd.close()
	
	if res: exit(0)
	else: exit(1)
