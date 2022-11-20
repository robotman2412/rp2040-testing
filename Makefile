
.PHONY: build flash clean gdb

DEBUGFLAGS=-DCMAKE_BUILD_TYPE=DEBUG \
			-DCMAKE_C_FLAGS_DEBUG="-ggdb" \
			-DCMAKE_CXX_FLAGS_DEBUG="-ggdb"

build:
	mkdir -p build
	make -C app/test1
	./embed2c.sh app/test1/build/output.elf elf_file > build/elf_file.c
	cd build; cmake $(DEBUGFLAGS) ..
	make -j$(shell nproc) -C build

flash: build
	picotool load -f build/rp2040test.uf2
	picotool reboot

clean:
	rm -rf build/*

gdb:
	gdb-multiarch -ex 'target remote localhost:3333' build/rp2040test.elf
