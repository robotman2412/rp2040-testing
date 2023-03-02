
#include "flash_bd.hpp"

#include <stdio.h>
#include <string.h>

// Create an SPI flash block device from a region of flash.
// Base and size must be a multiple of the block size (256).
FlashBD::FlashBD(off_t base, off_t size) {
	_blockSize = 4096;
	_blocks    = size / _blockSize;
	_base      = base;
	valid      = true;
	
	if (base % _blockSize) {
		printf("Error: FlashBD base (%u) not aligned to %u\n", size, _blockSize);
		valid = false;
	}
	if (size % _blockSize) {
		printf("Error: FlashBD size (%u) not aligned to %u\n", size, _blockSize);
		valid = false;
	}
}

// Read a single block from this device.
// This function may fail if length != blockSize.
FileError FlashBD::readBlock(off_t index, uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	
	// Assert index and length bounds.
	if (index + length / _blockSize > _blocks) return FileError::DISK_ERROR;
	
	// Flash is memory mapped, so just read with no penalty.
	uint32_t addr = index * _blockSize + _base + XIP_BASE;
	memcpy((void *) out, (const void *) addr, length);
}

// Write a single block to this device.
// This function may fail if length != blockSize.
FileError FlashBD::writeBlock(off_t index, uint8_t *in, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (length % _blockSize) return FileError::DISK_ERROR;
	
	// Assert index and length bounds.
	if (index + length / _blockSize > _blocks) return FileError::DISK_ERROR;
	
	// Compute address in flash.
	uint32_t addr = index * _blockSize + _base;
	// Write to flash.
	flash_range_erase(addr, length);
	flash_range_program(addr, in, length);
}
