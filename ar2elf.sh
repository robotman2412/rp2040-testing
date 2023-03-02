#!/bin/bash

# Usage: ar2elf.sh [infile.a] [outfile.elf] [linker]

infile=$(realpath "$1")
outfile=$(realpath "$2")
linker="$3"

tmpdir=$(mktemp -d)
cd "$tmpdir"
ar x "$infile"
$linker -r -o "$outfile" *
rm -rf "$tmpdir"
