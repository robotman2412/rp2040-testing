
#include "fatfs.hpp"
#include <blockdevice.hpp>

#define DEBUG

#ifdef DEBUG
#define debugf printf
#include "util.h"
#include "pico/stdlib.h"
#else
#define debugf(...) do{}while(0)
#define hexdump(...) do{}while(0)
#define sleep_ms(...) do{}while(0)
#endif

namespace Fat {



// Convert a name string to 8.3 format in an 11-char array.
void packName(std::string in, char out[11]) {
	// Start by blanking the output with space (0x20).
	memset((void *) out, 0x20, 11);
	
	// Whether the name was trimmed in any way.
	bool trimmed = false;
	
	// Remove trailing periods.
	// These don't count as trimming.
	std::size_t len = in.length();
	while (len > 0 && in[len-1] == '.') len --; trimmed = true;
	in = in.substr(0, len);
	
	// Find index of the last '.' character.
	std::size_t period = in.find_last_of('.');
	
	// Trim after to up to three characters.
	if (period < in.length() - 3) trimmed = true;
	for (uint8_t i = 0; i < 3 && i + period + 1 < in.length(); i++) {
		out[i + 8] = in[i + period + 1];
	}
	
	// If trimmed, add the decorator.
	if (trimmed && in.length() <= 6) {
		in += "~1";
	} else if (trimmed) {
		in = in.substr(0, 6);
		in += "~1";
	}
	
	// Take up to eight before characters.
	in = in.substr(period);
	if (in.length() > 8) {
		in = in.substr(0, 6);
		in += "~1";
	}
	for (uint8_t i = 0; i < 8 && i < in.length(); i++) {
		out[i] = in[i];
	}
	
	// Finally, convert to upper case.
	for (uint8_t i = 0; i < 11; i++) {
		if (out[i] >= 'a' && out[i] <= 'z') {
			out[i] += 'A' - 'a';
		}
	}
}

// Convert an 8.3 format name in an 11-char array to a string.
std::string unpackName(const char in[11]) {
	// Compute length for the '8' part of the name.
	uint8_t len8 = 8;
	while (len8 > 0 && in[len8-1] == ' ') len8 --;
	
	// Compute length for the '3' part of the name.
	uint8_t len3 = 3;
	while (len3 > 0 && in[len3+7] == ' ') len3 --;
	
	
	// Copy the '8' part into the string.
	std::string out;
	for (uint8_t i = 0; i < len8; i++) {
		out += in[i];
	}
	
	// Don't add anything else if there is no extension.
	if (!len3) return out;
	
	// Add the always-present '.' character.
	out += '.';
	
	// Copy the '3' part into the string.
	for (uint8_t i = 0; i < len3; i++) {
		out += in[i + 8];
	}
	
	return out;
}


// Case-insensitive string equality test.
bool iequals(const std::string &a, const std::string &b) {
	if (a.size() != b.size()) return false;
	for (std::size_t i = 0; i < a.size(); i++) {
		if (upper(a[i]) != upper(b[i])) return false;
	}
	return true;
}



// Extract the 13 characters into an array.
void LongNameEnt::getName(uint16_t out[13]) const {
	memcpy(out,    name1, 10);
	memcpy(out+5,  name2, 12);
	memcpy(out+11, name3,  4);
}

// Insert the 13 characters from an array.
void LongNameEnt::setName(const uint16_t in[13]) {
	memcpy(name1, in,    10);
	memcpy(name2, in+5,  12);
	memcpy(name3, in+11,  4);
}



// Set from a RawDitEnt.
FatDirEnt::FatDirEnt(const RawDirEnt &raw, off_t sectorsPerCluster, Type fsType) {
	// Placeholder values.
	owner = group = 1000;
	ownerAccess = groupAccess = globalAccess = AccessFlags{1,1,1};
	
	// FAT-specific values.
	if (fsType == Type::FAT32) {
		firstCluster = raw.fstClusLo | (raw.fstClusHi << 16);
	} else {
		firstCluster = raw.fstClusLo;
	}
	attr = raw.attr;
	
	// Translated values.
	name = unpackName(raw.name);
	isDirectory = attr & 0x10;
	size = raw.fileSize;
	diskSize = (size - 1) / sectorsPerCluster + 1;
}

// Set from a RawDirEnt with long name data.
FatDirEnt::FatDirEnt(const RawDirEnt &raw, const std::vector<LongNameEnt> &longName,
		off_t sectorsPerCluster, Type fsType) {
	// Placeholder values.
	owner = group = 1000;
	ownerAccess = groupAccess = globalAccess = AccessFlags{1,1,1};
	
	// FAT-specific values.
	if (fsType == Type::FAT32) {
		firstCluster = raw.fstClusLo | (raw.fstClusHi << 16);
	} else {
		firstCluster = raw.fstClusLo;
	}
	attr = raw.attr;
	
	// Translated values.
	isDirectory = attr & 0x10;
	size = raw.fileSize;
	diskSize = (size - 1) / sectorsPerCluster + 1;
	
	// The overcomplicated long name thing.
	std::vector<uint16_t> tmp;
	tmp.resize(longName.size() * 13);
	
	// Pass 1: Extract all the characters scattered throughout the `LongNameEnt`.
	for (uint8_t i = 0; i < longName.size(); i++) {
		// TODO: Enforce reverse order.
		// if (longName[i].ord & 0x3f != longName.size() - i) {/* Bad */}
		
		// Store its data into our temporary array.
		longName[longName.size()-i-1].getName(tmp.data() + i * 13);
	}
	
	// Pass 2: Convert into a UTF8 string.
	name.clear();
	for (uint16_t wide: tmp) {
		// The long name is null-terminated.
		if (!wide) break;
		
		if (wide <= 0x007f) {
			// 1 byte long encoding 0xxx xxxx.
			name += (char) wide;
			
		} else if (wide <= 0x07ff) {
			// 2 byte long encoding 110x xxxx  10xx xxxx.
			name += (char) (0xc0 | (wide >> 6 & 0x1f));
			name += (char) (0x80 | (wide      & 0x3f));
			
		} else {
			// 3 byte long encoding 1110 xxxx  10xx xxxx  10xx xxxx.
			name += (char) (0xe0 | (wide >> 12 & 0x0f));
			name += (char) (0xc0 | (wide >> 6  & 0x3f));
			name += (char) (0x80 | (wide       & 0x3f));
		}
	}
}


// Create a FAT.
FAT::FAT(BlockDevice &bd, off_t index, off_t size, Type type, bool cached):
	bd(bd), blockIndex(index), size(size), type(type), dirty(false) {
	
	// The next part doesn't apply if caching is off.
	if (!cached) return;
	
	// Determine amount of bytes to read.
	std::size_t read;
	if (type == Type::FAT12) {
		read = size * 3 / 2;
	} else if (type == Type::FAT16) {
		read = size * 2;
	} else /* type == Type::FAT32 */ {
		read = size * 4;
	}
	
	// Read it into a cache.
	bd.read(blockIndex * bd.blockSize(), (uint8_t *) cache.data(), read);
}


// Read an entry from the FAT.
uint32_t FAT::read(FileError &ec, off_t index) {
	if (cache.size()) {
		// Cached read.
		
		if (type == Type::FAT12) {
			// Complex read :/
			std::size_t offset = index * 3 / 2;
			uint16_t data = unaligned_read(*(uint16_t *) (cache.data() + offset));
			
			// Merge the appropriate bits.
			if (index & 1) {
				return data >> 4;
			} else {
				return data & 0x0fff;
			}
			
		} else if (type == Type::FAT16) {
			// Simple read.
			return ((uint16_t *) cache.data())[index];
			
		} else /* type == Type::FAT32 */ {
			// Simple read.
			return ((uint32_t *) cache.data())[index];
		}
		
	} else {
		// Direct read.
		
		if (type == Type::FAT12) {
			// Get 3-byte offset for the thing.
			off_t offset = blockIndex * bd.blockSize() + index * 3 / 2;
			
			// Complex reading shenanigans.
			uint16_t data;
			ec = bd.read(offset, (uint8_t *) &data, 2);
			if (ec) return Clusters::DEFECTIVE;
			
			// Merge the appropriate bits.
			if (index & 1) {
				return data >> 4;;
			} else {
				return data & 0x0fff;
			}
			
		} else if (type == Type::FAT16) {
			// Simple read.
			off_t offset = blockIndex * bd.blockSize() + index * 2;
			uint16_t data;
			ec = bd.read(offset, (uint8_t *) &data, 2);
			if (ec) return Clusters::DEFECTIVE;
			return data;
			
		} else /* type == Type::FAT32 */ {
			// Simple read.
			off_t offset = blockIndex * bd.blockSize() + index * 4;
			uint16_t data;
			ec = bd.read(offset, (uint8_t *) &data, 4);
			if (ec) return Clusters::DEFECTIVE;
			return data;
		}
	}
}

// Write an entry to the FAT.
void FAT::write(FileError &ec, off_t index, uint32_t value) {
	if (cache.size()) {
		// Cached write.
		
		std::size_t read;
		if (type == Type::FAT12) {
			// Complex write :/
			std::size_t offset = index * 3 / 2;
			uint16_t data = unaligned_read(*(uint16_t *) (cache.data() + offset));
			
			// Merge the appropriate bits.
			if (index & 1) {
				data = (data & 0x000f) | (value << 4);
			} else {
				data = (data & 0xf000) | (value & 0x0fff);
			}
			
			// Write back.
			unaligned_write(*(uint16_t *) (cache.data() + offset), data);
			
		} else if (type == Type::FAT16) {
			// Simple write.
			((uint16_t *) cache.data())[index] = value;
			
		} else /* type == Type::FAT32 */ {
			// Simple write.
			((uint32_t *) cache.data())[index] = value;
		}
		dirty = true;
		
	} else {
		// Direct write.
		
		if (type == Type::FAT12) {
			// Get 3-byte offset for the thing.
			off_t offset = blockIndex * bd.blockSize() + index * 3 / 2;
			
			// We always need to read before write.
			uint16_t data;
			ec = bd.read(offset, (uint8_t *) &data, 2);
			if (ec) return;
			
			// Merge the appropriate bits.
			if (index & 1) {
				data = (data & 0x000f) | (value << 4);
			} else {
				data = (data & 0xf000) | (value & 0x0fff);
			}
			
			// Write it back.
			ec = bd.write(offset, (uint8_t *) &data, 2);
			
		} else if (type == Type::FAT16) {
			// Direct write API.
			off_t offset = blockIndex * bd.blockSize() + index * 2;
			uint16_t data = value;
			ec = bd.write(offset, (uint8_t *) &data, sizeof(uint16_t));
			
		} else /* type == Type::FAT32 */ {
			// Direct write API.
			off_t offset = blockIndex * bd.blockSize() + index * 4;
			uint32_t data = value;
			ec = bd.write(offset, (uint8_t *) &data, sizeof(uint32_t));
		}
	}
}

// Save this to disk if cached.
void FAT::sync(FileError &ec) {
	if (dirty) {
		// Determine amount of bytes to write.
		std::size_t write;
		if (type == Type::FAT12) {
			write = size * 3 / 2;
		} else if (type == Type::FAT16) {
			write = size * 2;
		} else /* type == Type::FAT32 */ {
			write = size * 4;
		}
		
		ec = bd.write(blockIndex * bd.blockSize(), (uint8_t *) cache.data(), write);
	}
}



// Seek to the next cluster.
bool Stream::nextCluster(FileError &ec) {
	// Look it up in the FAT.
	uint32_t next = fs.fats[fs.activeFat].read(ec, cluster);
	if (ec) return false;
	
	// If it is EOF then there is corruption.
	if (next >= Clusters::END_OF_FILE) {
		ec = FileError::DISK_ERROR;
		return false;
	}
	
	// Otherwise, update current cluster with it.
	cluster = next;
	return true;
}


// Constructs a stream.
Stream::Stream(OpenMode mode, FatFS &fs, off_t cluster, off_t size):
	FileDesc(mode),
	fs(fs), bd(*fs.media), baseCluster(cluster), cluster(cluster), size(size) {
	pos = 0;
}


// Read bytes from this file.
// Returns read length.
int Stream::read(FileError &ec, char *out, int len) {
	if (!len) return 0;
	
	int read = 0;
	while (pos < size && read < len) {
		// Compute reading offset.
		// Subtract 2 from cluster accounts for the offset added by the design.
		off_t offset = fs.dataSectorIndex * bd.blockSize()
					+ (cluster - 2) * fs.clusterSize
					+ pos % fs.clusterSize;
		
		// Compute reading length.
		off_t leftInClus = fs.clusterSize - pos % fs.clusterSize;
		if (leftInClus > len - read) leftInClus = len - read;
		
		// Read from the media.
		ec = bd.read(offset, (uint8_t *) out, leftInClus);
		if (ec) return read;
		out  += leftInClus;
		read += leftInClus;
		pos  += leftInClus;
		
		// Test whether we need to load the next cluster.
		if (pos % fs.clusterSize == 0) {
			if (!nextCluster(ec)) return read;
		}
	}
	
	return read;
}

// Write bytes to this file.
// Returns written length.
int Stream::write(FileError &ec, const char *in, int len) {
	ec = FileError::READ_ONLY;
	return 0;
}

// Seeks in the file.
// Returns an error code.
int Stream::seek(FileError &ec, _fpos_t off, int whence) {
	// Compute target position.
	off_t target;
	switch (whence) {
		default: ec = FileError::INVALID_PARAM; return -1;
		case SEEK_CUR: target = pos + off; break;
		case SEEK_END: target = size + off; break;
		case SEEK_SET: target = off; break;
	}
	
	// Clamp target position to size.
	if (target > size) target = size;
	
	// Amount of clusters passed in the file.
	off_t targetClus  = target / fs.clusterSize;
	off_t currentClus = pos / fs.clusterSize;
	
	// If target < current cluster, reset position.
	if (targetClus < currentClus) {
		currentClus = 0;
		cluster = baseCluster;
	}
	
	// If target > current cluster, seek forward.
	while (targetClus > currentClus) {
		if (!nextCluster(ec)) return -1;
		currentClus ++;
	}
	
	// Update byte position.
	pos = target;
	return pos;
}

// Closes the file.
// Returns an error code.
int Stream::close(FileError &ec) {
	// TODO: Sync.
	return 0;
}

// Gets the absolute position in the file.
long Stream::tell() {
	return pos;
}



// Constructs a stream.
RootStream::RootStream(OpenMode mode, FatFS &fs, off_t sector, off_t size):
	FileDesc(mode),
	fs(fs), bd(*fs.media), sector(sector), size(size) {
	pos = 0;
}


// Read bytes from this file.
// Returns read length.
int RootStream::read(FileError &ec, char *out, int len) {
	// Compute offset.
	off_t offset = sector * bd.blockSize() + pos;
	if (len + pos > size) len = size - pos;
	
	// Do a simple read.
	ec = bd.read(offset, (uint8_t *) out, len);
	if (ec) return 0;
	pos += len;
	
	return len;
}

// Write bytes to this file.
// Returns written length.
int RootStream::write(FileError &ec, const char *in, int len) {
	// Compute offset.
	off_t offset = sector * bd.blockSize() + pos;
	if (len + pos > size) { ec = FileError::OUT_OF_SPACE; return 0; }
	
	// Do a simple write.
	ec = bd.write(offset, (const uint8_t *) in, len);
	if (ec) return 0;
	pos += len;
	
	return len;
}

// Seeks in the file.
// Returns an error code.
int RootStream::seek(FileError &ec, _fpos_t off, int whence) {
	// Simply edit the position.
	if (whence == SEEK_END) {
		pos = size + off;
	} else if (whence == SEEK_CUR) {
		pos += off;
	} else if (whence == SEEK_SET) {
		pos = off;
	}
	
	// Clamp the position.
	// No need to check negative; pos is unsigned.
	if (pos > size) pos = size;
	
	return pos;
}

// Closes the file.
// Returns an error code.
int RootStream::close(FileError &ec) {
	// TODO: Sync.
	return 0;
}

// Gets the absolute position in the file.
long RootStream::tell() {
	return pos;
}



// Interpret common BPB.
void FatFS::interpretBPBCommon(std::vector<uint8_t> &cache, BPBCommon *common, BPB16 *bpb16, BPB32 *bpb32) {
	// Check for signature.
	if (cache[510] != 0x55 && cache[511] != 0xAA) {
		printf("FAT signature missing (%02x %02x != 55 AA)\n", cache[510], cache[511]);
		valid = false; return;
	}
	else debugf("FAT signature valid.\n");
	
	
	// Number of sectors occupied by the root directory.
	rootDirSize    = unaligned_read(common->rootEntCnt);
	rootDirSectors = (rootDirSize * sizeof(RawDirEnt) - 1)
				/ unaligned_read(common->bytsPerSec) + 1;
	debugf("Root dir sectors: %u\n", rootDirSectors);
	
	
	// Number of sectors in the media.
	if (unaligned_read(common->FATSz16)) fatSectors = unaligned_read(common->FATSz16);
	else fatSectors = unaligned_read(bpb32->FATSz32);
	numFats = unaligned_read(common->numFATs);
	debugf("FAT count:        %u\n", numFats);
	debugf("FAT sectors:      %u\n", fatSectors);
	
	if (unaligned_read(common->totSec16)) sectors = unaligned_read(common->totSec16);
	else sectors = unaligned_read(common->totSec32);
	debugf("Total sectors:    %u\n", sectors);
	
	
	// Number of sectors left for data.
	dataSectors = sectors - unaligned_read(common->rsvdSecCnt)
				- numFats * fatSectors - rootDirSectors;
	debugf("Data sectors:     %u\n", dataSectors);
	
	// Get first data sector index.
	dataSectorIndex = unaligned_read(common->rsvdSecCnt)
				+ numFats * fatSectors + rootDirSectors;
	
	
	// Number of clusters.
	sectorsPerCluster = unaligned_read(common->secPerClus);
	clusters = dataSectors / sectorsPerCluster;
	debugf("Total clusters:   %u\n", clusters);
	
	
	// Determine filesystem type.
	if (clusters <= 4084) type = Type::FAT12;
	else if (clusters <= 65524) type = Type::FAT16;
	else type = Type::FAT32;
	switch (type) {
		case Type::FAT12: debugf("Filesystem type:  FAT12\n"); break;
		case Type::FAT16: debugf("Filesystem type:  FAT16\n"); break;
		case Type::FAT32: debugf("Filesystem type:  FAT32\n"); break;
	}
}

// Interpret FAT12 and FAT16 BPB.
void FatFS::interpretBPB16(std::vector<uint8_t> &cache, BPBCommon *common, BPB16 *bpb16) {
	// In FAT12 and FAT16, all FATs must be synchronised.
	activeFat    = 0;
	fatSync      = true;
	fatNeedsSync = true;
	
	// Get first fat index.
	fatSectorIndex  = unaligned_read(common->rsvdSecCnt);
	
	// Get root directory index.
	rootSectorIndex = fatSectorIndex + fatSectors * numFats;
}

// Interpret FAT32 BPB.
void FatFS::interpretBPB32(std::vector<uint8_t> &cache, BPBCommon *common, BPB32 *bpb32) {
	// In FAT32, somtimes synchronisation is optional.
	auto extFlags = unaligned_read(bpb32->extFlags);
	
	if (extFlags & 0x0080) {
		// Only one FAT is active, stored in bits 0-3.
		activeFat    = extFlags & 0x000f;
		fatSync      = false;
		fatNeedsSync = false;
	} else {
		// All FATs are active.
		activeFat    = 0;
		fatSync      = true;
		fatNeedsSync = true;
	}
	
	// Get root directory index.
	rootBaseCluster = unaligned_read(bpb32->rootClus);
	
	// Get first fat index.
	fatSectorIndex  = unaligned_read(common->rsvdSecCnt);
}


// Helper for getting a DirEnt from a file descriptor.
// Returns false when there are no more entries to read.
bool FatFS::dirNext(FatDirEnt &out, FileError &ec, FileDesc &fd) {
	union {
		RawDirEnt raw;
		LongNameEnt ln;
	};
	std::vector<LongNameEnt> longName;
	
	while (1) {
		// Try to read one RawDirEnt.
		int read = fd.read(ec, (char *) &raw, sizeof(raw));
		if (read != sizeof(raw)) {
			ec = FileError::DISK_ERROR;
		}
		if (ec || raw.name[0] == 0) {
			return false;
		}
		
		// Test for empty, but not last, entries.
		if (raw.name[0] == 0xE5) continue;
		// Push long name entries onto the list.
		if ((raw.attr & 0x0f) == 0x0f) {
			longName.push_back(ln);
			continue;
		}
		// Ignore volume labels.
		if (raw.attr & 0x08) continue;
		// Ignore `.` and `..` entries.
		if (raw.name[0] == '.') continue;
		
		// Otherwise, we have a valid raw entry to decode.
		if (longName.size())
			out = FatDirEnt(raw, longName, sectorsPerCluster, type);
		else
			out = FatDirEnt(raw, sectorsPerCluster, type);
		return true;
	}
}

// Search directory until `name` is found.
// Returns false when there is no match.
bool FatFS::dirSearch(FatDirEnt &out, FileError &ec, FileDesc &fd, const std::string &name) {
	FatDirEnt tmp;
	
	while (1) {
		// Get an entry.
		if (!dirNext(tmp, ec, fd)) {
			ec = FileError::NOT_FOUND;
			return false;
		}
		
		// Compare the name.
		if (iequals(tmp.name, name)) {
			// If it matches, simply return the thing.
			out = tmp;
			return true;
		}
	}
}

// Obtain a FileDesc for the parent directory of `path`.
std::unique_ptr<FileDesc> FatFS::dirOpen(FileError &ec, const Path &path, bool skipName) {
	// Start at root.
	std::unique_ptr<FileDesc> root;
	if (type == Type::FAT32) {
		root = std::make_unique<Stream>(
			Open::RB, *this, rootBaseCluster, 0x7fffffff
		);
	} else {
		root = std::make_unique<RootStream>(
			Open::RB, *this, rootSectorIndex, rootDirSize * sizeof(RawDirEnt)
		);
	}
	std::unique_ptr<FileDesc> dir;
	FileDesc *fd = root.get();
	
	// Keep entries to handle the `..` parts.
	std::vector<FatDirEnt> dirs;
	
	// Iterate directories.
	const auto &parts = path.parts();
	for (std::size_t i = 0; i < parts.size() - skipName; i ++) {
		const auto &name = parts[i];
		
		// Ignore when it is a `.` part.
		if (name == ".") continue;
		
		if (name == "..") {
			// Pop one when it is a `..` part.
			dirs.pop_back();
			
			if (dirs.size()) {
				// Re-open the upper dir.
				dir = std::make_unique<Stream>(open(dirs[dirs.size()-1], Open::RB));
				fd  = dir.get();
				
			} else {
				// Use the root dir.
				fd  = root.get();
			}
			fd->seek(ec, 0, SEEK_SET);
			
		} else {
			// Look up the directory in here.
			FatDirEnt entry;
			if (!dirSearch(entry, ec, *fd, name)) return nullptr;
			if (!entry.isDirectory) { 
				ec = FileError::NOT_A_DIR;
				return nullptr;
			}
			dirs.push_back(entry);
			
			// Open the new directory.
			dir = std::make_unique<Stream>(open(entry, Open::RB));
			fd  = dir.get();
		}
	}
	
	if (fd == root.get()) {
		return std::move(root);
	} else {
		return std::move(dir);
	}
}

// Create a file stream using a RawDirEnt.
Stream FatFS::open(FatDirEnt &entry, OpenMode mode) {
	return Stream(mode, *this, entry.firstCluster, entry.isDirectory ? 0x7fffffff : entry.size);
}

// Create a file stream using a FatDirEnt.
std::shared_ptr<FileDesc> FatFS::openShared(FatDirEnt &entry, OpenMode mode) {
	return std::make_shared<Stream>(mode, *this, entry.firstCluster, entry.isDirectory ? 0x7fffffff : entry.size);
}


// Try to mount the media.
// If mounting fails catastrophically, the FatFS is invalid.
// If some corruption is found but reading is possible, writing is disabled.
FatFS::FatFS(std::unique_ptr<BlockDevice> _media, bool _writable):
	media(std::move(_media)), writable(_writable) {
	valid = true;
	FileError ec = FileError::OK;
	
	// Read first 512 bytes from the media.
	std::vector<uint8_t> cache;
	cache.resize(512);
	ec = media->readBlock(0, cache.data(), 512);
	if (ec) {
		printf("Input/Output error.\n");
		valid = false; return;
	}
	
	
	// Check common BPB.
	auto common = (BPBCommon *) cache.data();
	auto bpb16  = (BPB16 *) (cache.data() + BPB16Offset);
	auto bpb32  = (BPB32 *) (cache.data() + BPB32Offset);
	interpretBPBCommon(cache, common, bpb16, bpb32);
	
	// Interpret type-specific parts.
	if (type == Type::FAT32) {
		interpretBPB32(cache, common, bpb32);
	} else {
		interpretBPB16(cache, common, bpb16);
	}
	clusterSize = media->blockSize() * sectorsPerCluster;
	
	
	// Set up FAT handles.
	for (uint8_t i = 0; i < numFats; i++) {
		fats.push_back(FAT(
			*media, fatSectorIndex + i, clusters, type, i == activeFat
		));
	}
	
	debugf("Data sect index:  %u\n", dataSectorIndex);
	debugf("Root dir index:   %u\n", rootSectorIndex);
	
	// Determine usage statistics.
	usedClusters  = 0;
	validClusters = clusters;
	for (off_t i = 0; i < clusters; i++) {
		// Checks an entry.
		uint32_t entry = fats[activeFat].read(ec, i);
		if (ec) {
			printf("Input/Output error\n");
			valid = false; return;
		}
		if (entry && entry <= Clusters::USED_END) usedClusters ++;
		if (entry && entry == Clusters::END_OF_FILE) usedClusters ++;
		if (entry == Clusters::DEFECTIVE) validClusters --;
	}
	debugf("Clusters used:    %03d%% (%u / %u)\n",
		usedClusters * 100 / validClusters, usedClusters, validClusters
	);
	debugf("Clusters broken:  %03d%% (%u / %u)\n",
		100 - validClusters * 100 / clusters, clusters-validClusters, clusters
	);
	
}


// List the files in a directory.
// The given path should already be in absolute form.
std::vector<DirEnt> FatFS::list(FileError &ec, const Path &path) {
	// Get a handle for reading the directory of `path`.
	auto fd = dirOpen(ec, path, false);
	
	// This vector shall be the output.
	std::vector<DirEnt> out;
	if (!fd) return out;
	
	// Finally, read all entries from the directory.
	FatDirEnt tmp;
	while (1) {
		if (!dirNext(tmp, ec, *fd)) return out;
		out.push_back(tmp);
	}
}

// Try to open a file in the filesystem.
// The given path should already be in absolute form.
std::shared_ptr<FileDesc> FatFS::open(FileError &ec, const Path &path, OpenMode mode) {
	// Get the directory handle for the dir the file is in.
	auto fd = dirOpen(ec, path, true);
	if (!fd) return nullptr;
	
	// Look up the file entry.
	const std::string &name = path.filename();
	FatDirEnt entry;
	if (!dirSearch(entry, ec, *fd, name)) {
		ec = FileError::NOT_FOUND;
		return nullptr;
	}
	
	// Does not permit writing.
	if (mode.write) {
		ec = FileError::READ_ONLY;
		return nullptr;
	}
	
	// Must not be a directory.
	if (entry.isDirectory) {
		ec = FileError::NOT_A_FILE;
		return nullptr;
	}
	
	// If found, then return our stream implementation.
	return openShared(entry, mode);
}

// Try to move a file from one path to another.
// The given paths should already be in absolute form.
bool FatFS::move(FileError &ec, const Path &source, const Path &dest) {
	ec = FileError::READ_ONLY;
	return false;
}

// Try to remove a file.
// The given path should already be in absolute form.
bool FatFS::remove(FileError &ec, const Path &path) {
	ec = FileError::READ_ONLY;
	return false;
}

// Force any cached writes to be written to the media immediately.
// You should call this occasionally to prevent data loss and also every time before shutdown.
bool FatFS::sync(FileError &ec) {
	ec = FileError::READ_ONLY;
	return false;
}




} // namespace FAT
