
.PHONY: build flash clean

build:
	mkdir -p build
	# make -C pax-graphics -f Pi_Pico.mk
	make -C app/test2
	./embed2c.sh app/test2/build/output.elf elf_file > build/elf_file.c
	cd build; cmake ..
	make -j$(shell nproc) -C build

flash: build
	picotool load -f build/rp2040test.uf2
	picotool reboot

clean:
	rm -rf build/*
	make -C pax-graphics -f Pi_Pico.mk clean
