
SOURCES		=$(shell find ./src -name '*.c')
ASSEMBLIES	=$(shell find ./src -name '*.s' -o -name '*.S')
HEADERS		=$(shell find ./src -name '*.h')
OBJECTS		=$(shell echo $(SOURCES)    | sed -e 's/src/build/g;s/\.c/.c.o/g')\
			 $(shell echo $(ASSEMBLIES) | sed -e 's/src/build/g;s/\.s/.s.o/g;s/\.S/.S.o/g')
OBJ_DEBUG	=$(shell echo $(OBJECTS)    | sed -e 's/\.o/.debug.o/g')
INCLUDES	=-Isrc -Ibuild -I../../shared
TARGET		=build/test1.elf
TARGET_DEBUG=build/test1.debug.elf

COMPILER		=arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb
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
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS_DEBUG) $(INCLUDES) -o $@.S $<

build/%.S.debug.o: src/%.S
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS_DEBUG) $(INCLUDES) -o $@ $<

build/%.c.o: src/%.c
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
	@$(COMPILER) -S $(CFLAGS) $(INCLUDES) -o $@.S $<

build/%.S.o: src/%.S
	@mkdir -p $(shell dirname $@)
	$(COMPILER) $(CFLAGS) $(INCLUDES) -o $@ $<
