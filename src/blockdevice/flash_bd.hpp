
#include "blockdevice.hpp"
#include "hardware/flash.h"
#include "hardware/address_mapped.h"

class FlashBD: public BlockDevice {
	protected:
		// It valid?
		bool valid;
		// Base address in flash.
		off_t _base;
		
	public:
		FlashBD(): valid(false) {}
		// Create an SPI flash block device from a region of flash.
		// Base and size must be a multiple of the block size (4096).
		FlashBD(off_t base, off_t size);
		
		// Read a single block from this device.
		// This function may fail if length != blockSize.
		FileError readBlock(off_t index, uint8_t *out, std::size_t length);
		// Write a single block to this device.
		// This function may fail if length != blockSize.
		FileError writeBlock(off_t index, uint8_t *in, std::size_t length);
};
