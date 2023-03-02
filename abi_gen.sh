#!/bin/bash

elftmp=$(mktemp)
symtmp=$(mktemp)
header="src/abi_generated.h"
abi_in="shared/abi_base.txt"
abi_out="shared/abi.txt"
abi_gen="shared/abi_gen.txt"

./ar2elf.sh build/pax-graphics/libpax_graphics.a $elftmp arm-none-eabi-gcc
./sym_extractor.py $elftmp $symtmp

syms=$(cat $symtmp $abi_gen)

echo '// WARNING: This is a generated file, do not edit it!' > $header
echo >> $header
echo '#ifdef GET_GENERATED_ABI_SYMBOLS' >> $header
echo '#undef GET_GENERATED_ABI_SYMBOLS' >> $header
for sym in $syms; do
	echo "extern void $sym();" >> $header
done
echo '#endif // GET_GENERATED_ABI_SYMBOLS' >> $header
echo >> $header
echo '#ifdef GET_GENERATED_ABI_LUT' >> $header
echo '#undef GET_GENERATED_ABI_LUT' >> $header
for sym in $syms; do
	echo "	{ \"$sym\", $sym }," >> $header
done
echo '#endif // GET_GENERATED_ABI_LUT' >> $header

cp $abi_in $abi_out
echo >> $abi_out
echo '# Begin generated section.' >> $abi_out
echo >> $abi_out
for sym in $syms; do
	echo $sym >> $abi_out
done
