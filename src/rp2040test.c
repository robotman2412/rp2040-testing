#include <stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "elfloader.h"

#define MPU_TYPE (*(uint32_t *) (PPB_BASE + 0xed90))
#define MPU_CTRL (*(uint32_t *) (PPB_BASE + 0xed94))
#define MPU_RNR  (*(uint32_t *) (PPB_BASE + 0xed98))
#define MPU_RBAR (*(uint32_t *) (PPB_BASE + 0xed9c))
#define MPU_RASR (*(uint32_t *) (PPB_BASE + 0xeda0))

#define M0_VTOR (*(uint32_t) (PPB_BASE + 0xed08))

extern const unsigned char elf_file[];
extern const unsigned int elf_file_length;

extern void simple();

/*

directory: "/home/julian/the_projects/rp2040test/build

command:
	/usr/bin/arm-none-eabi-gcc
	
	-DCFG_TUSB_DEBUG=0 -DCFG_TUSB_MCU=OPT_MCU_RP2040 -DCFG_TUSB_OS=OPT_OS_PICO -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1
	-DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FIX_RP2040_USB_DEVICE_ENUMERATION=1 -DLIB_PICO_FLOAT=1
	-DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1
	-DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1
	-DLIB_PICO_STDIO_USB=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CORE=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1
	-DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_UTIL=1 -DPICO_BOARD=\\\"pico_w\\\" -DPICO_BUILD=1
	-DPICO_CMAKE_BUILD_TYPE=\\\"Release\\\" -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0
	-DPICO_ON_DEVICE=1 -DPICO_PROGRAM_NAME=\\\"rp2040test\\\" -DPICO_PROGRAM_VERSION_STRING=\\\"0.1\\\" -DPICO_TARGET_NAME=\\\"rp2040test\\\"
	-DPICO_USE_BLOCKED_RAM=0
	
	-I/home/julian/the_projects/rp2040test -I/home/julian/the_projects/rp2040test/..
	-I/home/julian/pico/pico-sdk/src/common/pico_stdlib/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_gpio/include
	-I/home/julian/pico/pico-sdk/src/common/pico_base/include -I/home/julian/the_projects/rp2040test/build/generated/pico_base
	-I/home/julian/pico/pico-sdk/src/boards/include -I/home/julian/pico/pico-sdk/src/rp2_common/pico_platform/include
	-I/home/julian/pico/pico-sdk/src/rp2040/hardware_regs/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_base/include
	-I/home/julian/pico/pico-sdk/src/rp2040/hardware_structs/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_claim/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_sync/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_irq/include
	-I/home/julian/pico/pico-sdk/src/common/pico_sync/include -I/home/julian/pico/pico-sdk/src/common/pico_time/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_timer/include -I/home/julian/pico/pico-sdk/src/common/pico_util/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_uart/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_divider/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_runtime/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_clocks/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_resets/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_pll/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_vreg/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_watchdog/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/hardware_xosc/include -I/home/julian/pico/pico-sdk/src/rp2_common/pico_printf/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_bootrom/include -I/home/julian/pico/pico-sdk/src/common/pico_bit_ops/include
	-I/home/julian/pico/pico-sdk/src/common/pico_divider/include -I/home/julian/pico/pico-sdk/src/rp2_common/pico_double/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_int64_ops/include -I/home/julian/pico/pico-sdk/src/rp2_common/pico_float/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_malloc/include -I/home/julian/pico/pico-sdk/src/rp2_common/boot_stage2/include
	-I/home/julian/pico/pico-sdk/src/common/pico_binary_info/include -I/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_stdio_usb/include -I/home/julian/pico/pico-sdk/lib/tinyusb/src
	-I/home/julian/pico/pico-sdk/lib/tinyusb/src/common -I/home/julian/pico/pico-sdk/lib/tinyusb/hw
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include
	-I/home/julian/pico/pico-sdk/src/rp2_common/pico_unique_id/include -I/home/julian/pico/pico-sdk/src/rp2_common/hardware_flash/include
	-I/home/julian/pico/pico-sdk/src/common/pico_usb_reset_interface/include
	
	-mcpu=cortex-m0plus -mthumb -O3 -DNDEBUG
	
	-ffunction-sections -fdata-sections -std=gnu11
	
	-o CMakeFiles/rp2040test.dir/src/rp2040test.c.obj
	-c
	/home/julian/the_projects/rp2040test/src/rp2040test.c",

file: /home/julian/the_projects/rp2040test/src/rp2040test.c


*/

#include <elfloader_client_data.h>

void funny_abort() {
	printf("Abort called!\n");
	fflush(stdout);
	while (1) sleep_ms(1000);
}

int fancy_callback(int in) {
	printf("Callback %08x\n", in);
	fflush(stdout);
	sleep_ms(200);
	return 12;
}

void *funny_resolver(elf_loaded_t *loaded, const char *name, bool allow_object, bool allow_function) {
	printf("Resolver activated!\n");
	fflush(stdout);
	sleep_ms(200);
	return fancy_callback;
}

int main() {
	stdio_init_all();
	sleep_ms(2500);
	printf("\n\n\n\n\n\n\n\n\n\n\n\nStartup time!\n\n");
	sleep_ms(500);
	
	FILE *elf_fd = fmemopen((void *) elf_file, elf_file_length, "r");
	
	elf_load_sym_t api_load_sym_dummy[] = {
		{
			.parent      = &(elf_sym_t) {
				.name    = "callback",
				.type    = 0x02,
				.binding = 0x01,
				.info    = 0x12,
			},
			.vaddr       = &fancy_callback,
			.got_present = false,
			.got_addr    = 0,
		}
	};
	elf_loaded_t api_dummy = {
		.valid        = true,
		.num_symbols  = 1,
		.num_sections = 0,
		.symbols      = api_load_sym_dummy,
	};
	elf_loaded_t *api_dummy_arr[] = { &api_dummy };
	
	elf_ctx_t ctx = elf_interpret(elf_fd);
	elf_ctx_t *ctx_arr[1] = {&ctx};
	if (ctx.valid) {
		printf("Interpret success!\n");
		sleep_ms(500);
		elf_link_t link = elf_linked_load(1, ctx_arr, &elf_fd, 1, api_dummy_arr);
		
		if (link.valid) {
			printf("Dummy callback: %p\n", fancy_callback);
			printf("Link success!\n");
			sleep_ms(500);
			
			const char *sym_name = "quantum";
			int (*ptr)() = elf_adrof_sym(&link, sym_name, true, true);
			if (ptr) {
				printf("%s at %p\n", sym_name, ptr);
				sleep_ms(500);
				
				printf("Returns %08x\n", ptr());
			} else {
				printf("%s not found\n", sym_name);
			}
			
			// const char *sym_name = "lol";
			// int *ptr = elf_adrof_sym(&link, sym_name, true, true);
			// if (ptr) {
			// 	printf("%s at %p\n", sym_name, ptr);
			// 	sleep_ms(500);
				
			// 	printf("Value %08x\n", *ptr);
			// } else {
			// 	printf("%s not found\n", sym_name);
			// }
		} else {
			printf("Link error!\n");
		}
	} else {
		printf("Interpret error!\n");
	}
	fflush(stdout);
	
	while(1) sleep_ms(100);
}

/*
int main() {
	stdio_init_all();
	sleep_ms(2500);
	printf("\n\n\n\n\n\n\n\n\n\n\n\nStartup time!\n\n");
	sleep_ms(2000);
	
	FILE *elf_fd = fmemopen((void *) elf_file, elf_file_length, "r");
	
	elf_ctx_t ctx = elf_interpret(elf_fd);
	if (ctx.valid) {
		printf("Interpret success!\n");
		sleep_ms(2000);
		elf_loaded_t loaded = elf_load(elf_fd, &ctx);
		
		elf_client_data_t client_data = {
			.resolver_ctx = &loaded,
			.resolver     = funny_resolver,
			.abort        = funny_abort,
		};
		
		if (loaded.valid) {
			printf("Loaded at %08zx\n", loaded.vaddr);
			
			elf_client_entry_t entry = elf_adrof_sym(&loaded, "_start", true, true);
			if (entry) {
				printf("Entry at %p\n", entry);
				sleep_ms(2000);
				entry(client_data);
				printf("Done!\n");
			} else {
				printf("Entry not found!\n");
			}
		} else {
			printf("Load error!\n");
		}
	} else {
		printf("Interpret error!\n");
	}
	fflush(stdout);
	
	while(1) sleep_ms(100);
}
*/
