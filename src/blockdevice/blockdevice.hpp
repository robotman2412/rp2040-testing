
#pragma once

#include <stdint.h>
#include <vector>

#include <customio.hpp>

class BlockDevice {
	public:
		// Block/byte offset/index type.
		using off_t = uint32_t;
		
	protected:
		// Block size.
		off_t _blockSize;
		// Number of blocks.
		off_t _blocks;
		
		// Default constructor.
		BlockDevice() {}
		// Helper constructor.
		BlockDevice(off_t bs, off_t bc): _blockSize(bs), _blocks(bc) {}
		
	public:
		// I intend to VIRTUALISE this class.
		virtual ~BlockDevice() = default;
		
		// Read a single block from this device.
		// This function may fail if length != blockSize.
		virtual FileError readBlock(off_t index, uint8_t *out, std::size_t length) = 0;
		// Write a single block to this device.
		// This function may fail if length != blockSize.
		virtual FileError writeBlock(off_t index, const uint8_t *in, std::size_t length) = 0;
		// Force any cached writes to be written to the media immediately.
		// You should call this occasionally to prevent data loss and also every time before shutdown.
		virtual FileError sync() = 0;
		
		// Read a range of bytes from this block device.
		virtual FileError read(off_t offset, uint8_t *out, std::size_t length);
		// Write a range of bytes to this block device.
		virtual FileError write(off_t offset, const uint8_t *in, std::size_t length);
		
		// Get the size of a block in this device.
		// Must be a power of two.
		off_t blockSize() const { return _blockSize; }
		// Attempt to resize the block size.
		// This operation may fail if unaligned or the block size is unobtainable.
		virtual FileError setBlockSize(off_t newSize) = 0;
		// Get the number of blocks in this device.
		off_t blocks() const { return _blocks; }
		// Get the number of bytes in this device.
		off_t bytes() const { return blocks() * blockSize(); }
		// Get the block an index lies within.
		off_t offsetToBlock(off_t offset) { return offset / blockSize(); }
};
