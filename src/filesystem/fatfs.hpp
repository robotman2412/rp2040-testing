
#pragma once

#include "customio.hpp"
#include "blockdevice.hpp"
#include <string.h>
#include "unaligned_access.hpp"

namespace Fat {
class FatFS;


// Type of FAT filesystem.
enum Type {
	// The original, smallest FAT filesystem.
	// Has N <= 4084 clusters.
	FAT12,
	// A slightly larger variant of the FAT filesystem.
	// Has 4085 <= N <= 65524 clusters.
	FAT16,
	// The more modern, largest variant of the FAT filesystem.
	// Has N >= 65526 clusters.
	FAT32,
};

// Type of FAT32 entries.
// The top 4 bits are unused, and always zero.
namespace Clusters {
	
	// An unused cluster.
	static const uint32_t FREE        = 0x0000000;
	// Used cluster range start.
	static const uint32_t USED_BEGIN  = 0x0000002;
	// Used cluster range end.
	static const uint32_t USED_END    = 0xFFFFFF6;
	// Defective cluster.
	static const uint32_t DEFECTIVE   = 0xFFFFFF7;
	// End of file cluster.
	static const uint32_t END_OF_FILE = 0xFFFFFFF;
	
	// Translate FAT12 to FAT16 entry.
	static inline uint16_t fat12_to_fat16(uint16_t in) {
		if (in & 0x800) in |= 0xF000;
		return in;
	}
	// Translate FAT12 to FAT32 entry.
	static inline uint32_t fat12_to_fat32(uint16_t in) {
		if (in & 0x800) in |= 0xFFFF000;
		return in;
	}
	// Translate FAT16 to FAT32 entry.
	static inline uint32_t fat16_to_fat32(uint16_t in) {
		if (in & 0x8000) in |= 0xFFF0000;
		return in;
	}
	
	// Translate FAT16 to FAT12 entry.
	static inline uint16_t fat16_to_fat12(uint16_t in) {
		return in & 0xFFF;
	}
	// Translate FAT32 to FAT12 entry.
	static inline uint16_t fat32_to_fat12(uint32_t in) {
		return in & 0xFFF;
	}
	// Translate FAT32 to FAT16 entry.
	static inline uint16_t fat32_to_fat16(uint32_t in) {
		return in & 0xFFFF;
	}
}


// Common filesystem header information for all FAT types.
struct __attribute__((packed)) BPBCommon {
	// Jump to boot vector, either 0xEB ?? 0x90 or 0xE9 ?? ??.
	// Ignored by this driver.
	uint8_t jmpBoot[3];
	// Any data representing the name of an OEM.
	char oemName[8];
	// Bytes per sector, one of: 512, 1024, 2048, 4096.
	uint16_t bytsPerSec;
	// Sectors per cluster, power of 2 where 1 <= secPerClus <= 128.
	uint8_t secPerClus;
	// Number of reserved sectors, typically used to align sectors to clusters.
	uint16_t rsvdSecCnt;
	// Number of FATs, 2 is recommended but 1 is acceptable.
	uint8_t numFATs;
	// For FAT12 and FAT16, the number of entries in the root directory.
	// For FAT32, 0.
	uint16_t rootEntCnt;
	// The old 16-bit total sector count.
	uint16_t totSec16;
	// Media type, acceptable values are 0xF0 or 0xF8 through 0xFF.
	uint8_t media;
	// For FAT12 and FAT16, number of sectors reserved per FAT.
	// For FAT32, 0.
	uint16_t FATSz16;
	// Number of sectors per track for interrupt 0x13.
	uint16_t secPerTrk;
	// Number of heads for interrupt 0x13.
	uint16_t numHeads;
	// Number of hidden sectors, must be 0.
	uint32_t hiddSec;
	// The new 32-bit sector count.
	uint32_t totSec32;
};
static_assert(sizeof(BPBCommon) == 36, "BPBCommon must be 36 bytes in size.");

// Filesystem header information for FAT12 and FAT16.
struct __attribute__((packed)) BPB16 {
	// Interrupt 0x13 driver number.
	uint8_t drvNum;
	// Reserved, set to 0.
	uint8_t _reserved0;
	// Extended boot signature, 0x29 of either of the following two fields is nonzero.
	uint8_t bootSig;
	// Volume serial number.
	uint32_t volID;
	// Volume label.
	char volLab[11];
	// Informational filesystem type string.
	char filSysType[8];
	// Reserved, set to 0.
	uint8_t _reserved1[448];
	// Signature label, 0x55AA.
	uint16_t signatureWord;
};
static const std::size_t BPB16Offset = 36;
static_assert(sizeof(BPBCommon) + sizeof(BPB16) == 512, "BPB must be 512 bytes total in size.");

// Filesystem header information for FAT32.
struct __attribute__((packed)) BPB32 {
	// 32-bit number of sectors reserved per FAT.
	uint32_t FATSz32;
	// Flags, see above (FAT_ExtFlags_*).
	uint16_t extFlags;
	// Minor filesystem revision number.
	uint8_t FSVerMin;
	// Major filesystem revision number.
	uint8_t FSVerMaj;
	// Cluster index of the first cluster for the root directory.
	uint32_t rootClus;
	// Sector index of FSINFO structure.
	uint16_t FSInfo;
	// Index of the backup boot record.
	uint16_t bkBootSec;
	// Reserved, set to 0.
	uint8_t _reserved0[12];
	// Interrupt 0x13 driver number.
	uint8_t drvNum;
	// Reserved, set to 0.
	uint8_t _reserved1;
	// Extended boot signature, 0x29 of either of the following two fields is nonzero.
	uint8_t bootSig;
	// Volume serial number.
	uint32_t volID;
	// Volume label.
	char volLab[11];
	// Informational filesystem type string.
	char filSysType[8];
	// Reserved, set to 0.
	uint8_t _reserved2[420];
	// Signature label, 0x55AA.
	uint16_t signatureWord;
};
static const std::size_t BPB32Offset = 36;
static_assert(sizeof(BPBCommon) + sizeof(BPB16) == 512, "BPB must be 512 bytes total in size.");


// Convert a name string to 8.3 format in an 11-char array.
// Does not handle invalid names.
void packName(std::string in, char out[11]);
// Convert an 8.3 format name in an 11-char array to a string.
std::string unpackName(char in[11]);


// Directory entry as stored on disk.
struct FatDirEnt {
	// Short filename (8.3 format).
	char name[11];
	// Attribute flags.
	uint8_t attr;
	// Reserved, set to 0.
	uint8_t _reserved;
	// Tenths of a second creation time (0 <= N <= 199).
	uint8_t crtTimeTenth;
	// Two seconds creation time.
	uint16_t crtTime;
	// Creation date.
	uint16_t crtDate;
	// FAT32 only: High half of first cluster index.
	uint16_t fstClusHi;
	// Two seconds write time.
	uint16_t wrtTime;
	// Write date.
	uint16_t wrtDate;
	// Low half of first cluster index.
	uint16_t fstClusLo;
	// Byte size of the file.
	uint32_t fileSize;
	
	// Set name from a string.
	void setName(const std::string &in) { packName(in, name); }
	// Convert name to string.
	std::string getName() { return unpackName(name); }
};
static_assert(sizeof(FatDirEnt) == 32, "FatDirEnt must be 32 bytes in size.");


// The FAT access helper class.
class FAT {
	protected:
		// The block device to read from.
		BlockDevice &bd;
		// Block index of this FAT.
		off_t blockIndex;
		// Number of entries in this FAT.
		off_t size;
		// Type of FAT to act as.
		Type type;
		// This FAT's cache, if any.
		std::vector<uint8_t> cache;
		// Whether the cache is both present and dirty.
		bool dirty;
		
	public:
		// Create a FAT.
		FAT(BlockDevice &bd, off_t index, off_t size, Type type, bool cached);
		
		// Read an entry from the FAT.
		uint32_t read(FileError &ec, off_t index);
		// Write an entry to the FAT.
		void write(FileError &ec, off_t index, uint32_t value);
		// Save this to disk if cached.
		void sync(FileError &ec);
};


// The implementation of the file descriptor.
class Stream: public FileDesc {
	protected:
		// The associated block device.
		BlockDevice &bd;
		// The associated filesystem.
		FatFS &fs;
		// The initial cluster index.
		off_t baseCluster;
		// The current cluster index.
		off_t cluster;
		// The current byte position.
		off_t pos;
		// The current file size.
		off_t size;
		// TODO: Write capability.
		
	public:
		// Constructs a stream.
		Stream(OpenMode mode, FatFS &fs, off_t cluster, off_t size);
		
		// Read bytes from this file.
		// Returns read length.
		int read(FileError &ec, char *out, int len);
		// Write bytes to this file.
		// Returns written length.
		int write(FileError &ec, const char *in, int len);
		// Seeks in the file.
		// Returns 0 on success, -1 on error.
		int seek(FileError &ec, _fpos_t off, int whence);
		// Closes the file.
		// Returns 0 on success, -1 on error.
		int close(FileError &ec);
};

// The special stream for FAT12 and FAT16 root directory.
class RootStream: public FileDesc {
	protected:
		// The associated block device.
		BlockDevice &bd;
		// The associated filesystem.
		FatFS &fs;
		// The initial sector index.
		off_t sector;
		// The current byte position.
		off_t pos;
		// The current file size.
		off_t size;
		// TODO: Write capability.
		
	public:
		// Constructs a stream.
		RootStream(OpenMode mode, FatFS &fs, off_t sector, off_t size);
		
		// Read bytes from this file.
		// Returns read length.
		int read(FileError &ec, char *out, int len);
		// Write bytes to this file.
		// Returns written length.
		int write(FileError &ec, const char *in, int len);
		// Seeks in the file.
		// Returns 0 on success, -1 on error.
		int seek(FileError &ec, _fpos_t off, int whence);
		// Closes the file.
		// Returns 0 on success, -1 on error.
		int close(FileError &ec);
};

// The FAT filesystem driver.
class FatFS: public Filesystem {
	protected:
		friend class FAT;
		friend class Stream;
		friend class RootStream;
		
		// Is valid?
		bool valid;
		// Type of FAT.
		Type type;
		// Allow writing?
		bool writable;
		// The raw storage for the filesystem.
		std::unique_ptr<BlockDevice> media;
		
		// Number of entries in the root directory, if any.
		off_t rootDirSize;
		// Number of sectors occupied by the root directory, if any.
		off_t rootDirSectors;
		// Number of FATs.
		uint8_t numFats;
		// Number of sectors occupied by each fat.
		off_t fatSectors;
		
		// Number of sectors per cluster.
		off_t sectorsPerCluster;
		// Total number of sectors.
		off_t sectors;
		// Number of sectors in the data region.
		off_t dataSectors;
		// Total number of clusters in the data region.
		off_t clusters;
		
		// Number of used clusters as per the FAT.
		off_t usedClusters;
		// How many non-defective clusters exist.
		off_t validClusters;
		// How big one cluster is.
		off_t clusterSize;
		
		// Sector index of the root directory.
		off_t rootSectorIndex;
		// Sector index of the first FAT.
		off_t fatSectorIndex;
		// Sector index of the first data sector.
		off_t dataSectorIndex;
		
		// Handles for all FATs.
		std::vector<FAT> fats;
		// Index of the active FAT.
		uint8_t activeFat;
		// Whether all FATs are synced.
		bool fatSync;
		// Whether all FATs need to be synced before unmount.
		bool fatNeedsSync;
		
		// Interpret common BPB.
		void interpretBPBCommon(std::vector<uint8_t> &cache, BPBCommon *common, BPB16 *bpb16, BPB32 *bpb32);
		// Interpret FAT12 and FAT16 BPB.
		void interpretBPB16(std::vector<uint8_t> &cache, BPBCommon *common, BPB16 *bpb16);
		// Interpret FAT32 BPB.
		void interpretBPB32(std::vector<uint8_t> &cache, BPBCommon *common, BPB32 *bpb32);
		
		// Helper for getting a DirEnt from a file descriptor.
		// Sets the FatDirEnt to null when there are no more entries to read.
		std::pair<DirEnt, FatDirEnt> dirNext(FileError &ec, FileDesc &fd);
		// Search directory until `name` is found.
		// Sets the FatDirEnt to null when there is no match.
		std::pair<DirEnt, FatDirEnt> dirSearch(FileError &ec, FileDesc &fd, std::string name);
		// Create a file stream using a FatDirEnt.
		Stream open(FatDirEnt &entry, OpenMode mode);
		
	public:
		// Does nothing by default.
		FatFS(): valid(false) {}
		// Try to mount the media.
		// If mounting fails catastrophically, the FatFS is invalid.
		// If some corruption is found but reading is possible, writing is disabled.
		FatFS(std::unique_ptr<BlockDevice> media, bool writable=true);
		
		// List the files in a directory.
		// The given path should already be in absolute form.
		std::vector<DirEnt> list(FileError &ec, const Path &path);
		// Try to open a file in the filesystem.
		// The given path should already be in absolute form.
		std::shared_ptr<FileDesc> open(FileError &ec, const Path &path, OpenMode mode);
		// Try to move a file from one path to another.
		// The given paths should already be in absolute form.
		bool move(FileError &ec, const Path &source, const Path &dest);
		// Try to remove a file.
		// The given path should already be in absolute form.
		bool remove(FileError &ec, const Path &path);
		// Force any cached writes to be written to the media immediately.
		// You should call this occasionally to prevent data loss and also every time before shutdown.
		bool sync(FileError &ec);
};

} // namespace Fat

using FatFS = Fat::FatFS;
