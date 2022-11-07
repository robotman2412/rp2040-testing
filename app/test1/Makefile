
SOURCES		=$(shell find ./src -name '*.c')
ASSEMBLIES	=$(shell find ./src -name '*.s' -o -name '*.S')
HEADERS		=$(shell find ./src -name '*.h')
OBJECTS		=$(shell echo $(SOURCES)    | sed -e 's/src/build/g;s/\.c/.c.o/g')\
			 $(shell echo $(ASSEMBLIES) | sed -e 's/src/build/g;s/\.s/.s.o/g;s/\.S/.S.o/g')
OBJ_DEBUG	=$(shell echo $(OBJECTS)    | sed -e 's/\.o/.debug.o/g')
INCLUDES	=-Isrc -Ibuild
TARGET		=build/test1.elf
TARGET_DEBUG=build/test1.debug.elf

COMPILER		=arm-none-eabi-gcc
CFLAGS			=-fPIC -fno-hosted -nostdlib -nodefaultlibs -c
CFLAGS_DEBUG	=$(CFLAGS) -g -DDEBUG
LDFLAGS			=-fPIC -fno-hosted -nostdlib -nodefaultlibs
LDFLAGS_DEBUG	=$(CFLAGS) -g -DDEBUG

.PHONY: all debug clean

all: $(TARGET)
	cp $< build/output.elf

debug: $(TARGET_DEBUG)
	cp $< build/output.elf

clean:
	rm -rf build/*


$(TARGET): $(OBJECTS)
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(LDFLAGS) -o $@ $^

$(TARGET_DEBUG): $(OBJ_DEBUG)
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(LDFLAGS_DEBUG) -o $@ $^


build/%.c.debug.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@$(COMPILER) -S $(CFLAGS_DEBUG) -o $@.S $<
	$(COMPILER) $(CFLAGS_DEBUG) -o $@ $<

build/%.S.debug.o: src/%.S
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) -o $@ $<

build/%.c.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@$(COMPILER) -S $(CFLAGS) -o $@.S $<
	$(COMPILER) $(CFLAGS) -o $@ $<

build/%.S.o: src/%.S
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) -o $@ $<
