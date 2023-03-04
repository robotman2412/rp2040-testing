
#include "blockdevice.hpp"
#include "hardware/flash.h"
#include "hardware/address_mapped.h"
#include <map>
#include <vector>

class RomBD: public BlockDevice {
	protected:
		// It valid?
		bool valid;
		// The array with data in it.
		const uint8_t *data;
		
	public:
		RomBD(): valid(false) {}
		// Create a block device from an array of raw data.
		// Size must be a multiple of the block size.
		// Block size must be a power of 2.
		RomBD(const uint8_t *data, off_t blockSize, off_t size);
		
		// Read a single block from this device.
		// This function may fail if length != blockSize.
		FileError readBlock(off_t index, uint8_t *out, std::size_t length);
		// Write a single block to this device.
		// This function may fail if length != blockSize.
		FileError writeBlock(off_t index, const uint8_t *in, std::size_t length) { return FileError::NOT_SUPPORTED; }
		// Force any cached writes to be written to the media immediately.
		// You should call this occasionally to prevent data loss and also every time before shutdown.
		FileError sync() { return FileError::OK; }
		
		// Read a range of bytes from this block device.
		FileError read(off_t offset, uint8_t *out, std::size_t length);
		// Write a range of bytes to this block device.
		FileError write(off_t offset, const uint8_t *in, std::size_t length) { return FileError::NOT_SUPPORTED; }
		
		// Attempt to resize the block size.
		// This operation may fail if unaligned or the block size is unobtainable.
		FileError setBlockSize(off_t newSize);
};
