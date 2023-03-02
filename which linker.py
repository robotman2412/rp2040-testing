#!/usr/bin/env python3

# pip install pyelftools

from sys import argv
from elftools.elf.elffile import *
import os, subprocess

infiles = [
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/crti.o',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/crtbegin.o',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/crt0.o',
	'CMakeFiles/rp2040test.dir/src/main.c.obj',
	'CMakeFiles/rp2040test.dir/src/elfloader.c.obj',
	'CMakeFiles/rp2040test.dir/src/abi_impl.c.obj',
	'CMakeFiles/rp2040test.dir/src/ili9341/ili9341.c.obj',
	'CMakeFiles/rp2040test.dir/elf_file.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_stdlib/stdlib.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_gpio/gpio.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_claim/claim.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_platform/platform.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_sync/sync.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_irq/irq.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_irq/irq_handler_chain.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_sync/sem.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_sync/lock_core.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_time/time.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_time/timeout_helper.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_timer/timer.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_util/datetime.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_util/pheap.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_util/queue.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_sync/mutex.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/common/pico_sync/critical_section.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_uart/uart.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_divider/divider.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_runtime/runtime.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_clocks/clocks.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_pll/pll.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_vreg/vreg.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_watchdog/watchdog.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_xosc/xosc.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_printf/printf.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_bootrom/bootrom.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_divider/divider.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_double/double_aeabi.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_double/double_init_rom.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_double/double_math.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_double/double_v1_rom_shim.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_float/float_aeabi.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_float/float_init_rom.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_float/float_math.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_float/float_v1_rom_shim.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_malloc/pico_malloc.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_standard_link/crt0.S.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_standard_link/new_delete.cpp.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_standard_link/binary_info.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio/stdio.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb_descriptors.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/device/usbd.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/device/usbd_control.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/audio/audio_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/cdc/cdc_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/dfu/dfu_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/dfu/dfu_rt_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/hid/hid_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/midi/midi_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/msc/msc_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/net/ecm_rndis_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/net/ncm_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/usbtmc/usbtmc_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/vendor/vendor_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/class/video/video_device.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/tusb.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/lib/tinyusb/src/common/tusb_fifo.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/pico_unique_id/unique_id.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_flash/flash.c.obj',
	'CMakeFiles/rp2040test.dir/home/julian/pico/pico-sdk/src/rp2_common/hardware_spi/spi.c.obj',
	'pax-graphics/libpax_graphics.a',
	'/tmp/cc8CpmY1.o',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libstdc++.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libm.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/libgcc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libc.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/../../../arm-none-eabi/lib/thumb/v6-m/nofp/libnosys.a',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/crtend.o',
	'/usr/lib/gcc/arm-none-eabi/10.3.1/thumb/v6-m/nofp/crtn.o'
]

def filter_name(name: str):
	return False

def get_symbols(elf: ELFFile):
	"""Gets all symbols in the elf file"""
	syms = []
	
	for sect in elf.iter_sections():
		if not isinstance(sect, SymbolTableSection): continue
		for symbol in sect.iter_symbols():
			syms.append(symbol)
	
	return syms

def get_defined_symbols(syms: list):
	"""Gets all defined symbols from a list of symbols"""
	out = []
	
	for sym in syms:
		if sym["st_shndx"] != "SHN_UNDEF":
			out.append(sym)
	
	return out

def process_ar(filename: str, sym_names: list[str]):
	"""Unpacks the archive to a temp directory and then processes each."""
	# Make a temp dir.
	tmpdir = "/tmp/whichlinker"
	subprocess.run(["mkdir", "-p", tmpdir])
	# Clean it out.
	for file in os.listdir(tmpdir):
		os.remove(tmpdir + "/" + file)
	# Populate it with new files using ar.
	cwd = os.getcwd()
	filename = os.path.abspath(filename)
	os.chdir(tmpdir)
	subprocess.run(["ar", "x", filename])
	os.chdir(cwd)
	# Run over all the things.
	for file in os.listdir(tmpdir):
		process_file(tmpdir + "/" + file, sym_names, filename + ": " + file)
		

def process_file(filename: str, sym_names: list[str], source=None):
	"""Tries to look for `sym_name` in ELF file `filename`."""
	try:
		if source == None:
			source = filename
		fd=open(filename, "rb")
		try:
			elf=ELFFile(fd)
			defs=get_defined_symbols(get_symbols(elf))
			for sym in defs:
				if sym.name in sym_names or filter_name(sym.name):
					print(sym.name + " found in " + source)
		except:
			pass
		fd.close()
	except:
		pass

if __name__ == "__main__":
	os.chdir("build")
	sym_names = argv[1:]
	for filename in infiles:
		if filename.endswith(".a"):
			process_ar(filename, sym_names)
		else:
			process_file(filename, sym_names)
