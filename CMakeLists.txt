# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Override linker script
set(PICO_TARGET_LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "$ENV{HOME}/pico/pico-sdk")

set(PICO_BOARD pico_w CACHE STRING "Board type")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
	message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(rp2040test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
add_executable(rp2040test
	src/main.c
	src/elfloader.c
	src/abi_impl.c
	src/ili9341/ili9341.c
	build/elf_file.c
)

# LINKER script will be PICO_TARGET_LINKER_SCRIPT if set on target, or ${CMAKE_CURRENT_LIST_DIR}/memmap_foo.ld
# if PICO_TARGET_BINARY_TYPE is set to foo on the target, otherwise ${CMAKE_CURRENT_LIST_DIR}/memmap_${PICO_DEFAULT_BINARY_TYPE).ld
# target_link_options(pico_standard_link INTERFACE
# 	"LINKER:--script=memmap_custom.ld>"
# )

pico_set_program_name(rp2040test "rp2040test")
pico_set_program_version(rp2040test "0.1")

pico_enable_stdio_uart(rp2040test 0)
pico_enable_stdio_usb(rp2040test 1)

# Add the standard library to the build
target_link_libraries(rp2040test pico_stdlib hardware_spi)

# Add the standard include files to the build
target_include_directories(rp2040test PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/src
	${CMAKE_CURRENT_LIST_DIR}/src/ili9341
	${CMAKE_CURRENT_LIST_DIR}/src/ili9341/include
	${CMAKE_CURRENT_LIST_DIR}/shared
	${CMAKE_CURRENT_LIST_DIR}/pax-graphics/src
	${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(rp2040test ${CMAKE_CURRENT_LIST_DIR}/pax-graphics/build/pax_graphics)

pico_add_extra_outputs(rp2040test)

