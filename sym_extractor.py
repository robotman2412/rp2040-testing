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

def get_defined_symbols(syms: list):
	"""Gets all defined symbols from a list of symbols"""
	out = []
	
	for sym in syms:
		if sym["st_info"]["bind"] == "STB_GLOBAL" \
			and sym["st_shndx"] != "SHN_UNDEF":
			out.append(sym)
	
	return out

def process_file(elf_in_fd, abi_out_fd):
	"""Process an ELF file from beginning to end"""
	# Do some parsing and checks.
	elf = ELFFile(elf_in_fd)
	# Get all symbols.
	syms = get_symbols(elf)
	# Cherry-pick defined symbols.
	defs = get_defined_symbols(syms)
	# Turn it into a list of just the names.
	names = [ x.name for x in defs ]
	# Sort the names.
	names.sort()
	# And output them all to the output FD.
	for name in names:
		abi_out_fd.write(name)
		abi_out_fd.write('\n')
	return True

if __name__ == "__main__":
	if len(argv) != 3:
		print("Usage: sym_extractor.py [infile.elf] [outfile.txt]")
		exit(1)
	in_fd  = open(argv[1], "rb")
	out_fd = open(argv[2], "w")
	process_file(in_fd, out_fd)
	in_fd.close()
	out_fd.close()
