
#include "blockdevice.hpp"
#include "hardware/flash.h"
#include "hardware/address_mapped.h"
#include <map>
#include <vector>

class FlashBD: public BlockDevice {
	public:
		// Write cache data type.
		using Page = uint8_t[256];
		// Write cache type (page index to page data).
		using WriteCache = std::map<off_t, Page>;
		
	protected:
		// It valid?
		bool valid;
		// Base address in flash.
		off_t _base;
		// Number of pages in a block.
		off_t pagePerBlock;
		// Maximum number of 256-byte pages in write cache.
		off_t cacheLimit;
		
		// Page erase status vector.
		// Initially, it is assumed no pages are erased.
		std::vector<bool> erasedPages;
		// Write cache.
		// When reading, this is checked first.
		WriteCache writeCache;
		// Sync a specific write cache entry.
		bool sync(WriteCache::iterator entry);
		// Sync, but only for when the writeCache is too big.
		bool syncExcess();
		// Get or copy a page to cache, then return the iterator for the entry.
		WriteCache::iterator encache(off_t page);
		// Erase a sector of flash, preserving data that need be.
		bool erase(off_t sector);
		// Read a specific page of data from flash.
		void readPage(off_t page, Page &out);
		// Read a specific page of data from cache first, flash second.
		// Length may not exceed 256.
		void readCached(off_t page, std::size_t index, uint8_t *out, std::size_t len);
		
	public:
		FlashBD(): valid(false) {}
		// Create a built-in flash block device from a region of flash.
		// Base and size must be a multiple of the block size.
		// Base must also be a multiple of 4096.
		// Block size must be a power of 2 >= 256.
		FlashBD(off_t blockSize, off_t base, off_t size);
		
		// Read a single block from this device.
		// This function may fail if length != blockSize.
		FileError readBlock(off_t index, uint8_t *out, std::size_t length);
		// Write a single block to this device.
		// This function may fail if length != blockSize.
		FileError writeBlock(off_t index, const uint8_t *in, std::size_t length);
		// Force any cached writes to be written to the media immediately.
		// You should call this occasionally to prevent data loss and also every time before shutdown.
		FileError sync();
		
		// Read a range of bytes from this block device.
		FileError read(off_t offset, uint8_t *out, std::size_t length);
		// Write a range of bytes to this block device.
		FileError write(off_t offset, const uint8_t *out, std::size_t length);
		
		// Attempt to resize the block size.
		// This operation may fail if unaligned or the block size is unobtainable.
		FileError setBlockSize(off_t newSize);
};
