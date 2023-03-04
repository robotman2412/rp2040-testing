
#include "rom_bd.hpp"
#include <string.h>



// Create a block device from an array of raw data.
// Size must be a multiple of the block size.
// Block size must be a power of 2.
RomBD::RomBD(const uint8_t *data, off_t blockSize, off_t size) {
	_blockSize = blockSize;
	_blocks    = size / _blockSize;
	valid      = true;
	this->data = data;
	
	// Check parameters.
	if (_blockSize & (_blockSize - 1)) {
		printf("Error: RomBD block size (%u) is not a power of 2\n", _blockSize);
		valid = false;
	}
	if (size % _blockSize) {
		printf("Error: RomBD size (%u) not aligned to %u\n", size, _blockSize);
		valid = false;
	}
}


// Read a single block from this device.
// This function may fail if length != blockSize.
FileError RomBD::readBlock(off_t index, uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (index + length / _blockSize > _blocks) return FileError::INVALID_PARAM;
	
	// Just memcpy() it lol.
	memcpy((void *) out, (const void *) (data + index * _blockSize), length);
	return FileError::OK;
}


// Read a range of bytes from this block device.
FileError RomBD::read(off_t offset, uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (offset + length >= _blockSize * _blocks) return FileError::INVALID_PARAM;
	
	// Just memcpy() it lol.
	memcpy((void *) out, (const void *) (data + offset), length);
	return FileError::OK;
}


// Attempt to resize the block size.
// This operation may fail if unaligned or the block size is unobtainable.
FileError RomBD::setBlockSize(off_t newSize) {
	if (!valid) return FileError::DISK_ERROR;
	
	// Check parameters.
	if (newSize & (newSize - 1)) {
		printf("Error: RomBD new block size (%u) is not a power of 2\n", newSize);
		return FileError::INVALID_PARAM;
	}
	if (_blocks * _blockSize % newSize) {
		printf("Error: RomBD size (%u) not aligned to %u\n", _blocks * _blockSize, newSize);
		return FileError::INVALID_PARAM;
	}
	
	// Apply changes.
	_blocks = _blocks * _blockSize / newSize;
	_blockSize = newSize;
	
	return FileError::OK;
}
