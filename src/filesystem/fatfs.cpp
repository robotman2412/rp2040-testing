
#include "fatfs.hpp"
#include <blockdevice.hpp>

#define DEBUG

#ifdef DEBUG
#define debugf printf
#else
#define debugf(...) do{}while(0)
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
		in = in.substr(6);
		in += "~1";
	}
	
	// Take up to eight before characters.
	in = in.substr(period);
	if (in.length() > 8) {
		in = in.substr(6);
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
std::string unpackName(char in[11]) {
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
		// Test whether we need to load the next cluster.
		if ((pos + 1) % fs.clusterSize == 0) {
			printf("Next cluster.\n");
			
			// Look it up in the FAT.
			uint32_t next = fs.fats[fs.activeFat].read(ec, cluster);
			if (ec) return read;
			
			// If it is EOF then there is corruption.
			if (next >= Clusters::END_OF_FILE) {
				ec = FileError::DISK_ERROR; return read;
			}
			
			// Otherwise, update current cluster with it.
			cluster = next;
		}
		
		// Read as much from this cluster as possible.
		off_t offset = fs.dataSectorIndex * bd.blockSize()
					+ cluster * fs.clusterSize
					+ pos % fs.clusterSize;
		off_t leftInClus = fs.clusterSize - pos % fs.clusterSize;
		if (leftInClus > len - read) leftInClus = len - read;
		printf("Reading %u from offset %u\n", leftInClus, offset);
		ec = bd.read(offset, (uint8_t *) out, leftInClus);
		out  += leftInClus;
		read += leftInClus;
		if (ec) return read;
	}
	
	return read;
}

// Write bytes to this file.
// Returns written length.
int Stream::write(FileError &ec, const char *in, int len) {
	errno = (int) FileError::READ_ONLY;
	return 0;
}

// Seeks in the file.
// Returns an error code.
int Stream::seek(FileError &ec, _fpos_t off, int whence) {
	errno = (int) FileError::NOT_SUPPORTED;
	return -1;
}

// Closes the file.
// Returns an error code.
int Stream::close(FileError &ec) {
	// TODO: Sync.
	return 0;
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
	
	return 0;
}

// Closes the file.
// Returns an error code.
int RootStream::close(FileError &ec) {
	// TODO: Sync.
	return 0;
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
	rootDirSectors = (rootDirSize * sizeof(FatDirEnt) - 1)
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
	rootSectorIndex = unaligned_read(bpb32->rootClus) * sectorsPerCluster;
	
	// Get first fat index.
	fatSectorIndex  = unaligned_read(common->rsvdSecCnt);
}


// Helper for getting a DirEnt from a file descriptor.
// Sets the FatDirEnt to null when there are no more entries to read.
std::pair<DirEnt, FatDirEnt> FatFS::dirNext(FileError &ec, FileDesc &fd) {
	std::pair<DirEnt, FatDirEnt> out;
	DirEnt    &decoded = out.first;
	FatDirEnt &raw     = out.second;
	
	while (1) {
		// Try to read one FatDirEnt.
		int read = fd.read(ec, (char *) &raw, sizeof(raw));
		if (read != sizeof(raw)) ec = FileError::DISK_ERROR;
		if (ec || raw.name[0] == 0) {
			memset((void *) &raw, 0, sizeof(raw));
			return out;
		}
		
		// Test for empty, but not last, entries.
		if (raw.name[0] == 0xE5) continue;
		// Ignore volume labels.
		if (raw.attr & 0x08) continue;
		// Ignore long name entries for now.
		if ((raw.attr & 0x0f) == 0x0f) continue;
		
		// Otherwise, we have a valid raw entry to decode.
		decoded.name         = raw.getName();
		decoded.owner        = 0;
		decoded.group        = 0;
		decoded.ownerAccess  = AccessFlags{1, 1, 1};
		decoded.groupAccess  = AccessFlags{1, 1, 1};
		decoded.globalAccess = AccessFlags{1, 1, 1};
		decoded.isDirectory  = raw.attr & 0x10;
		decoded.size         = raw.fileSize;
		decoded.diskSize     = ((raw.fileSize - 1) / clusterSize + 1) * clusterSize;
		return out;
	}
}

// Search directory until `name` is found.
// Sets the FatDirEnt to null when there is no match.
std::pair<DirEnt, FatDirEnt> FatFS::dirSearch(FileError &ec, FileDesc &fd, std::string name) {
	std::pair<DirEnt, FatDirEnt> out;
	DirEnt    &decoded = out.first;
	FatDirEnt &raw     = out.second;
	
	// Get the reduced name form.
	char packedName[11];
	packName(name, packedName);
	
	while (1) {
		// Get an entry.
		out = dirNext(ec, fd);
		// If it is empty, pass along the empty.
		if (raw.name[0] == 0) return out;
		
		// Compare the name.
		if (!memcmp((const void *) packedName, (const void *) raw.name, 11)) {
			// If it matches, simply return the thing.
			return out;
		}
	}
}

// Create a file stream using a FatDirEnt.
Stream FatFS::open(FatDirEnt &entry, OpenMode mode) {
	off_t cluster;
	// FAT12 and FAT16 shall ignore fstClusHi.
	if (type == Type::FAT32) {
		cluster = ((uint32_t) entry.fstClusHi << 16) | entry.fstClusLo;
	} else {
		cluster = entry.fstClusLo;
	}
	// Subtract 2 from cluster accounts for the offset added by the design.
	return Stream(mode, *this, cluster - 2, entry.fileSize);
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
	// Start at root.
	std::unique_ptr<FileDesc> fd = std::make_unique<RootStream>(RootStream(
		Open::RB, *this, rootSectorIndex, rootDirSize * sizeof(FatDirEnt)
	));
	
	// TODO: Subdirectories.
	
	// Finally, read all entries from the directory.
	std::vector<DirEnt> out;
	std::pair<DirEnt, FatDirEnt> pair;
	while (1) {
		pair = dirNext(ec, *fd);
		if (ec) return out;
		if (!pair.second.name[0]) return out;
		out.push_back(pair.first);
	}
}

// Try to open a file in the filesystem.
// The given path should already be in absolute form.
std::shared_ptr<FileDesc> FatFS::open(FileError &ec, const Path &path, OpenMode mode) {
	
}

// Try to move a file from one path to another.
// The given paths should already be in absolute form.
bool FatFS::move(FileError &ec, const Path &source, const Path &dest) {
	
}

// Try to remove a file.
// The given path should already be in absolute form.
bool FatFS::remove(FileError &ec, const Path &path) {
	
}

// Force any cached writes to be written to the media immediately.
// You should call this occasionally to prevent data loss and also every time before shutdown.
bool FatFS::sync(FileError &ec) {
	
}




} // namespace FAT
