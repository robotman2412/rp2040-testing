
.PHONY: build flash clean

build:
	mkdir -p build
	make -C app/test2
	./embed2c.sh app/test2/build/output.elf elf_file > build/elf_file.c
	./embed2c.sh fatty.iso fatty_iso > build/fatty_iso.c
	cd build; cmake ..
	make -j$(shell nproc) -C build

flash: build
	picotool load -f build/rp2040test.uf2
	picotool reboot

clean:
	rm -rf build/*
