
SOURCES		=$(shell find ./src -name '*.c')
ASSEMBLIES	=$(shell find ./src -name '*.s' -o -name '*.S')
HEADERS		=$(shell find ./src -name '*.h')
OBJECTS		=$(shell echo $(SOURCES)    | sed -e 's/src/build/g;s/\.c/.c.o/g')\
			 $(shell echo $(ASSEMBLIES) | sed -e 's/src/build/g;s/\.s/.s.o/g;s/\.S/.S.o/g')
OBJ_DEBUG	=$(shell echo $(OBJECTS)    | sed -e 's/\.o/.debug.o/g')
INCLUDES	=-Isrc -Ibuild -I../../shared
TARGET		=build/test1.elf
TARGET_DEBUG=build/test1.debug.elf

PTRGEN          =../../ptrtab_gen.py
COMPILER		=arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb
CFLAGS			=-fno-hosted -nostdlib -nodefaultlibs -c
CFLAGS_DEBUG	=$(CFLAGS) -g -DDEBUG
LDFLAGS			=-fno-hosted -nostdlib -nodefaultlibs -T linker.ld
LDFLAGS_DEBUG	=$(CFLAGS) -g -DDEBUG

.PHONY: all debug clean

all: $(TARGET) build/ptrtab.s
	$(COMPILER) $(LDFLAGS) -o build/output.elf $^

debug: $(TARGET_DEBUG) build/ptrtab.debug.s
	$(COMPILER) $(LDFLAGS_DEBUG) -o build/output.elf $^

clean:
	rm -rf build/*


build/ptrtab.s: $(TARGET) $(OBJECTS)
	$(PTRGEN) $< $@ ../../shared/abi.txt

build/ptrtab.debug.s: $(TARGET_DEBUG) $(OBJ_DEBUG)
	$(PTRGEN) $< $@ ../../shared/abi.txt


$(TARGET): $(OBJECTS)
	@mkdir -p $(shell dirname $@)
	$(COMPILER) -r $(LDFLAGS) -o $@ $^

$(TARGET_DEBUG): $(OBJ_DEBUG)
	@mkdir -p $(shell dirname $@)
	$(COMPILER) -r $(LDFLAGS_DEBUG) -o $@ $^


build/%.c.debug.o: src/%.c Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS_DEBUG) $(INCLUDES) -o $@.S $<

build/%.S.debug.o: src/%.S Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<

build/%.c.o: src/%.c Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS) $(INCLUDES) -o $@.S $<

build/%.S.o: src/%.S Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
