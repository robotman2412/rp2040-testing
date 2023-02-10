
SOURCES		=$(shell find ./src -name '*.c' -o -name '*.cpp')
ASSEMBLIES	=$(shell find ./src -name '*.s' -o -name '*.S')
HEADERS		=$(shell find ./src -name '*.h' -o -name '*.hpp') \
			 $(shell find ../../shared -name '*.h')
OBJECTS		=$(shell echo '$(SOURCES) '   | sed -e 's/src/build/g;s/\.cpp /.cpp.o /g;s/\.c /.c.o /g')\
			 $(shell echo $(ASSEMBLIES) | sed -e 's/src/build/g;s/\.s/.s.o/g;s/\.S/.S.o/g')
OBJ_DEBUG	=$(shell echo $(OBJECTS)    | sed -e 's/\.o/.debug.o/g')
INCLUDES	=-Isrc -Ibuild -I../../shared/include
TARGET		=build/test1.elf
TARGET_DEBUG=build/test1.debug.elf

PTRGEN          =../../ptrtab_gen.py
COMPILER		=arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb
CFLAGS			=-nostdlib -nodefaultlibs -c -fno-exceptions
CFLAGS_DEBUG	=$(CFLAGS) -g -DDEBUG
LDFLAGS			=-nostdlib -nodefaultlibs -T linker.ld
LDFLAGS_DEBUG	=$(LDFLAGS) -g

.PHONY: all debug clean

all: $(TARGET) build/ptrtab.o
	$(COMPILER) $(LDFLAGS) -o build/output.elf $^

debug: $(TARGET_DEBUG) build/ptrtab.debug.o
	$(COMPILER) $(LDFLAGS_DEBUG) -o build/output.elf $^

clean:
	rm -rf build/*


build/abi.o:
	$(COMPILER) $(CFLAGS) $(shell find ../../shared -name '*.c') -o build/abi.o

build/ptrtab.o: build/ptrtab.s
	$(COMPILER) $(CFLAGS) -o $@ $<

build/ptrtab.debug.o: build/ptrtab.debug.s
	$(COMPILER) $(CFLAGS) -o $@ $<

build/ptrtab.s: $(TARGET) $(OBJECTS)
	$(PTRGEN) $< $@ ../../shared/abi.txt

build/ptrtab.debug.s: $(TARGET_DEBUG) $(OBJ_DEBUG)
	$(PTRGEN) $< $@ ../../shared/abi.txt


$(TARGET): $(OBJECTS) build/abi.o
	@mkdir -p $(shell dirname $@)
	$(COMPILER) -r -o $@ $^

$(TARGET_DEBUG): $(OBJ_DEBUG) build/abi.o
	@mkdir -p $(shell dirname $@)
	$(COMPILER) -r -o $@ $^


build/%.cpp.debug.o: src/%.cpp Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS_DEBUG) $(INCLUDES) -o $@.S $<

build/%.c.debug.o: src/%.c Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS_DEBUG) $(INCLUDES) -o $@.S $<

build/%.S.debug.o: src/%.S Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<

build/%.cpp.o: src/%.cpp Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS) $(INCLUDES) -o $@.S $<

build/%.c.o: src/%.c Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS) $(INCLUDES) -o $@.S $<

build/%.S.o: src/%.S Makefile
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
