
#include "blockdevice.hpp"
#include <string.h>

// Like `memcpy`, but with uint8_t.
static inline void bytecopy(uint8_t *dest, const uint8_t *src, std::size_t len) {
	memcpy((void *) dest, (const void *) src, len);
}

// Read a range of bytes from this block device.
FileError BlockDevice::read(off_t offset, uint8_t *out, std::size_t length) {
	FileError ec = FileError::OK;
	if (!length) return ec;
	
	// Create a read cache.
	uint8_t *cache = new uint8_t[_blockSize];
	
	// Read first block.
	off_t error = offset % _blockSize;
	off_t index = offset / _blockSize;
	off_t i = _blockSize - error;
	ec = read(index++, cache, _blockSize);
	if (ec) goto exit;
	bytecopy(out + error, cache, _blockSize - error);
	
	// Read intermediate blocks.
	for (; i <= length - _blockSize; i += _blockSize) {
		ec = read(index++, cache, _blockSize);
		if (ec) goto exit;
		bytecopy(out + i, cache, _blockSize);
	}
	
	// Read last block, if any.
	if (i < length) {
		ec = read(index, cache, _blockSize);
		if (ec) goto exit;
		bytecopy(out + i, cache, length - i);
	}
	
	// Clean up.
	exit:
	delete[] cache;
	return ec;
}

// Write a range of bytes to this block device.
FileError BlockDevice::write(off_t offset, const uint8_t *in, std::size_t length) {
	FileError ec = FileError::OK;
	if (!length) return ec;
	
	// Create a read cache.
	uint8_t *cache = new uint8_t[_blockSize];
	
	// Modify first block.
	off_t error = offset % _blockSize;
	off_t index = offset / _blockSize;
	off_t i = _blockSize - error;
	if (error) ec = read(index, cache, _blockSize);
	if (ec) goto exit;
	bytecopy(cache + error, in, _blockSize - error);
	ec = write(index++, cache, _blockSize);
	if (ec) goto exit;
	
	// Write intermediate blocks.
	for (; i <= length - _blockSize; i += _blockSize) {
		ec = write(index++, in + i, _blockSize);
		if (ec) goto exit;
	}
	
	// Modify last block, if any.
	if (i < length) {
		if (error) ec = read(index, cache, _blockSize);
		if (ec) goto exit;
		bytecopy(cache, in, length - i);
		ec = write(index, cache, _blockSize);
		if (ec) goto exit;
	}
	
	// Clean up.
	exit:
	delete[] cache;
	return ec;
}
