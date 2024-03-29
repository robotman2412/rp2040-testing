cmake_minimum_required(VERSION 3.10)

# Set the C and C++ compilers to use.
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
# These don't pass built-in cmake test, so disable said test.
set(CMAKE_C_COMPILER_FORCED true)
set(CMAKE_CXX_COMPILER_FORCED true)

# Define the project.
project(test2 LANGUAGES C CXX ASM)

# Compiler options: Exceptions disabled, no system libraries.
add_compile_options(-mcpu=cortex-m0plus -mthumb -std=gnu11 -nostdlib -nodefaultlibs -fno-exceptions)
# Linker options: Relocatable, no system libraries
add_link_options(-nostdlib -nodefaultlibs -r)

# Add ABI files into include path.
include_directories(
	../abi/include
	../../../pax-graphics/src
	../../../pax-graphics/src/cpp
)

# Create output file and add sources.
add_executable(main.o
	src/main.cpp
)
