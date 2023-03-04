
#include "flash_bd.hpp"

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/sync.h>



// Sync a specific write cache entry.
bool FlashBD::sync(WriteCache::iterator entry) {
	off_t index = entry->first;
	Page &data  = entry->second;
	
	// Ensure page is erased before trying to write.
	if (!erasedPages[index]) {
		bool res = erase(index / 16);
		if (!res) return false;
	}
	
	// Write data to the page now.
	off_t addr = _base + index * 256;
	uint32_t irqs = save_and_disable_interrupts();
	flash_range_program(addr, data, 256);
	restore_interrupts(irqs);
	
	// Remove iterator from the map.
	writeCache.erase(entry);
	
	return true;
}

// Sync, but only for when the writeCache is too big.
bool FlashBD::syncExcess() {
	// Make sure the write cache isn't too big.
	while (writeCache.size() > cacheLimit) {
		// We keep taking the iterator because `sync(iter)` removes from the map.
		auto iter = writeCache.begin();
		if (!sync(iter)) return false;
	}
	
	return true;
}

// Get or copy a page to cache, then return the iterator for the entry.
FlashBD::WriteCache::iterator FlashBD::encache(off_t page) {
	auto iter = writeCache.find(page);
	
	if (iter != writeCache.end()) {
		// It is already in the cache.
		return iter;
		
	} else {
		// Make a new entry.
		Page &data = writeCache[page];
		readPage(page, data);
		return writeCache.find(page);
	}
}

// Erase a sector of flash, preserving data that need be.
bool FlashBD::erase(off_t sector) {
	// Evaluate which pages we need to send back into the write cache.
	for (off_t i = sector * 16; i < sector * 16 + 16; i++) {
		// Unerased pages not in the write cache shall be preserved.
		if (!erasedPages[i]) {
			// Read this into the write cache.
			auto iter = encache(i);
			if (iter == writeCache.end()) return false;
		}
	}
	
	// Erase flash now.
	off_t addr = _base + sector * 4096;
	uint32_t irqs = save_and_disable_interrupts();
	flash_range_erase(addr, 4096);
	restore_interrupts(irqs);
	
	// Mark affected pages as erased.
	for (off_t i = sector * 16; i < sector * 16 + 16; i++) {
		erasedPages[i] = true;
	}
	
	return true;
}

// Read a specific page of data.
void FlashBD::readPage(off_t index, Page &out) {
	off_t addr = index * 256 + _base + XIP_BASE;
	memcpy((void *) out, (const void *) addr, 256);
}

// Read a specific page of data from cache first, flash second.
// Length may not exceed 256.
void FlashBD::readCached(off_t page, std::size_t index, uint8_t *out, std::size_t len) {
	auto iter = writeCache.find(page);
	if (index >= 256) return;
	if (index + len > 256) len = 256 - index;
	if (iter == writeCache.end()) {
		// Not in cache, read from flash.
		off_t addr = page * 256 + _base + XIP_BASE;
		memcpy((void *) (index + out), (const void *) (index + addr), len);
		
	} else {
		// Read from cache.
		memcpy((void *) (index + out), (const void *) (index + iter->second), len);
	}
}



// Create a built-in flash block device from a region of flash.
// Base and size must be a multiple of the block size.
// Base must also be a multiple of 4096.
// Block size must be a power of 2 >= 256.
FlashBD::FlashBD(off_t blockSize, off_t base, off_t size) {
	_blockSize = blockSize;
	_blocks    = size / _blockSize;
	_base      = base;
	valid      = true;
	cacheLimit = 16;
	
	// Check parameters.
	if (_blockSize < 256 || (_blockSize & (_blockSize - 1))) {
		printf("Error: FlashBD block size (%u) is not a power of 2 >= 256\n", _blockSize);
		valid = false;
	}
	if (base % 4096) {
		printf("Error: FlashBD base (%u) not aligned to 4096\n", size);
		valid = false;
	} else if (base % _blockSize) {
		printf("Error: FlashBD base (%u) not aligned to %u\n", base, _blockSize);
		valid = false;
	}
	if (size % _blockSize) {
		printf("Error: FlashBD size (%u) not aligned to %u\n", size, _blockSize);
		valid = false;
	}
	
	// Allocate bits for erased status.
	off_t sectorCount = (size - 1) / 4096 + 1;
	// There are 16 pages per sector.
	if (valid) erasedPages.resize(sectorCount * 16);
	// Calculate pages per block.
	pagePerBlock = blockSize / 256;
}



// Read a single block from this device.
// This function may fail if length != blockSize.
FileError FlashBD::readBlock(off_t index, uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	
	// Assert index and length bounds.
	if (index + length / _blockSize > _blocks) return FileError::INVALID_PARAM;
	
	// Read in parts due to write cache.
	for (off_t i = 0; i < length; i += 256) {
		off_t page = i / 256 + index * pagePerBlock;
		readCached(page, 0, out + i, length - i);
	}
	
	return FileError::OK;
}

// Write a single block to this device.
// This function may fail if length != blockSize.
FileError FlashBD::writeBlock(off_t index, const uint8_t *in, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (length % _blockSize) return FileError::INVALID_PARAM;
	
	// Assert index and length bounds.
	if (index + length / _blockSize > _blocks) return FileError::INVALID_PARAM;
	
	// Append to the write cache.
	off_t page = index * pagePerBlock;
	for (off_t i = 0; i < pagePerBlock; i++) {
		// Get or create the write cache entry.
		Page &data = writeCache[i];
		// Put in the new data.
		memcpy((void *) data, (const void *) (in + i * 256), 256);
	}
	
	// Make sure the write cache isn't too big.
	if (!syncExcess()) return FileError::DISK_ERROR;
	
	return FileError::OK;
}

// Force any cached writes to be written to the media immediately.
// You should call this occasionally to prevent data loss and also every time before shutdown.
FileError FlashBD::sync() {
	if (!valid) return FileError::DISK_ERROR;
	
	// Call sync on all entries in the write cache.
	while (!writeCache.empty()) {
		// We keep taking the iterator because `sync(iter)` removes from the map.
		auto iter = writeCache.begin();
		if (!sync(iter)) return FileError::DISK_ERROR;
	}
	
	return FileError::OK;
}



// Read a range of bytes from this block device.
FileError FlashBD::read(off_t offset, uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (offset + length >= _blockSize * _blocks) return FileError::INVALID_PARAM;
	if (!length) return FileError::OK;
	
	// First page in the thing.
	std::size_t i = 0;
	off_t    page = offset / 256;
	readCached(page++, offset % 256, out, length);
	i += 256 - offset % 256;
	
	// Any remaining pages.
	for (; i < length; i += 256) {
		readCached(page++, 0, out + i, length - i);
	}
	
	return FileError::OK;
}

// Write a range of bytes to this block device.
FileError FlashBD::write(off_t offset, const uint8_t *out, std::size_t length) {
	if (!valid) return FileError::DISK_ERROR;
	if (offset + length >= _blockSize * _blocks) return FileError::INVALID_PARAM;
	
	// First page in the thing.
	std::size_t i = 0;
	off_t    page = offset / 256;
	auto     iter = encache(page++);
	if (iter == writeCache.end()) return FileError::DISK_ERROR;
	
	// Determine length to copy into page.
	std::size_t cpy = length < 256 ? length : 256;
	cpy -= (offset % 256);
	
	// Copy into write cache entry.
	memcpy((void *) (iter->second + offset % 256), (const void *) out, cpy);
	i += cpy;
	
	// Any remaining pages.
	for (; i < length; i += 256) {
		iter = encache(page++);
		if (iter == writeCache.end()) return FileError::DISK_ERROR;
		
		// Copy into write cache entry.
		cpy = length - i < 256 ? length - i : 256;
		memcpy((void *) iter->second, (const void *) (out + i), cpy);
	}
	
	// Make sure the write cache isn't too big.
	if (!syncExcess()) return FileError::DISK_ERROR;
	
	return FileError::OK;
}



// Attempt to resize the block size.
// This operation may fail if unaligned or the block size is unobtainable.
FileError FlashBD::setBlockSize(off_t newSize) {
	if (!valid) return FileError::DISK_ERROR;
	
	// Check parameters.
	if (newSize < 256 || (newSize & (newSize - 1))) {
		printf("Error: FlashBD new block size (%u) is not a power of 2 >= 256\n", newSize);
		return FileError::INVALID_PARAM;
	}
	if (_base % _blockSize) {
		printf("Error: FlashBD base (%u) not aligned to %u\n", _base, newSize);
		return FileError::INVALID_PARAM;
	}
	if (_blocks * _blockSize % newSize) {
		printf("Error: FlashBD size (%u) not aligned to %u\n", _blocks * _blockSize, newSize);
		return FileError::INVALID_PARAM;
	}
	
	// Apply changes.
	_blocks = _blocks * _blockSize / newSize;
	_blockSize = newSize;
	pagePerBlock = _blockSize / 256;
	
	return FileError::OK;
}
