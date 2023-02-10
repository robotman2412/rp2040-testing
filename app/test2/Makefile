
COMPILER=arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb
PTRGEN  =../../ptrtab_gen.py
ABI     =abi/abi.txt

.PHONY: all clean build/app/main.o build/abi/abi.o

all: build/output.elf

clean:
	rm -rf build


# Combine main+library with ptrtab into final executable.
build/output.elf: build/main_and_abi.o build/ptrtab.o
	@mkdir -p build
	$(COMPILER) -nostdlib -nodefaultlibs -o $@ -T linker.ld $^

# Combine main and library.
build/main_and_abi.o: build/app/main.o build/abi/abi.o
	$(COMPILER)	-r -o $@ $^

# Forward job for main to cmake.
build/app/main.o:
	@mkdir -p build/app
	@cd build/app; cmake ../../app
	@$(MAKE) -C build/app

# Forward job for library to cmake.
build/abi/abi.o:
	@mkdir -p build/abi
	@cd build/abi; cmake ../../abi
	@$(MAKE) -C build/abi

# Assemble ptrtab.s into ptrtab.o.
build/ptrtab.o: build/ptrtab.s
	$(COMPILER) -o $@ -c $<

# Generate ptrtab.s for main+library.
build/ptrtab.s: build/main_and_abi.o
	@mkdir -p build
	$(PTRGEN) $< $@ $(ABI)
