
#pragma once

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>



enum FileError {
	// Nothing bad happened.
	OK = 0,
	// No such file or directory.
	NOT_FOUND = ENOENT,
	// Out of disk space.
	OUT_OF_SPACE = ENOSPC,
	// Out of memory.
	OUT_OF_MEM = ENOMEM,
	// Not a directory.
	NOT_A_DIR = ENOTDIR,
	// Not a file.
	NOT_A_FILE = EISDIR,
	// Permission denied.
	NO_PERM = EACCES,
	// Not supported.
	NOT_SUPPORTED = ENOTSUP,
	// Filename too long.
	NAME_TOO_LONG = ENAMETOOLONG,
	// Disk I/O error.
	DISK_ERROR = EIO,
};

struct OpenMode {
	// Open for reading.
	bool read;
	// Open for writing.
	bool write;
	// Open in append mode (don't truncate; start at the end).
	bool append;
	// Interpret as binary file.
	bool binary;
	
	// Parse from a string.
	static OpenMode parse(std::string in);
};


class FileDesc {
	protected:
		// Read allowed?
		bool allowRead;
		// Write allowed?
		bool allowWrite;
		// Treat as binary?
		bool binary;
		// Is this file still open?
		bool open;
		
		// A little default.
		FileDesc(OpenMode mode):
			allowRead(mode.read),
			allowWrite(mode.append || mode.write),
			binary(mode.binary) {}
		
	public:
		// I intend to VIRTUALISE this class.
		virtual ~FileDesc() = default;
		
		// Read allowed?
		bool isRead()      { return allowRead; }
		// Write allowed?
		bool isWrite()     { return allowWrite; }
		// Is both read and write?
		bool isReadWrite() { return allowRead && allowWrite; }
		// Treat as binary?
		bool isBinary()    { return binary; }
		// Is this file still open?
		bool isOpen()      { return open; }
		
		// Read bytes from this file.
		// Returns read length.
		virtual int read(char *out, int len) = 0;
		// Write bytes to this file.
		// Returns written length.
		virtual int write(const char *in, int len) = 0;
		// Seeks in the file.
		// Returns an error code.
		virtual int seek(_fpos_t off, int whence) = 0;
		// Closes the file.
		// Returns an error code.
		virtual int close() = 0;
};


struct AccessFlags {
	// Is reading allowed?
	bool read;
	// Is writing allowed?
	bool write;
	// Is running as program allowed?
	bool execute;
};


struct DirEnt {
	// Filename this entry refers to.
	std::string name;
	// User ID of the file's owner.
	int owner;
	// Group ID of the file's group.
	int group;
	// Access permissions for the owner.
	AccessFlags ownerAccess;
	// Access permissions for the group.
	AccessFlags groupAccess;
	// Access permissions publicly.
	AccessFlags globalAccess;
	// Is the file a directory?
	bool isDirectory;
	// Size of the file.
	std::size_t size;
	// Size on disk.
	std::size_t diskSize;
};


class Path {
	protected:
		// String representation.
		std::string str;
		// Pieces of path.
		std::vector<std::string> _parts;
		// Whether this path is a relative one.
		bool _relative;
		
	public:
		// Make from string.
		Path(const std::string &in);
		// Make from string.
		Path(const char *in = "/"): Path(std::string(in)) {}
		// Make from list of parts.
		Path(const std::vector<std::string> &in, bool relative);
		// Make from another path.
		Path(const Path &other) = default;
		
		// Get the string representation.
		const std::string &string() const { return str; }
		// Set from string representation.
		void string(std::string newValue);
		// Get the parts representation.
		const std::vector<std::string> &parts() const { return _parts; }
		// Set from parts representation.
		void parts(std::vector<std::string> newValue);
		// Is this path relative?
		bool relative() const { return _relative; }
		// Set relative.
		void relative(bool newValue);
		
		// Test whether this path starts with another.
		bool startsWith(const Path &other) const;
		
		// Concatenate to another path.
		Path operator+(const Path &other) const;
		// Get a sub-path.
		Path substr(std::size_t start, std::size_t len = (std::size_t) -1) const;
		
		// The equality test.
		bool operator==(const char *other) const { return str == other; }
		// The equality test.
		bool operator==(const Path &other) const { return str == other.str; }
		// The equality test.
		bool operator==(const std::string &other) const { return str == other; }
		
		// The equality test.
		bool operator!=(const char *other) const { return str != other; }
		// The equality test.
		bool operator!=(const Path &other) const { return str != other.str; }
		// The equality test.
		bool operator!=(const std::string &other) const { return str != other; }
};


class Filesystem {
	public:
		// I intend to VIRTUALISE this class.
		virtual ~Filesystem() = default;
		
		// List the files in a directory.
		// The given path should already be in absolute form.
		virtual std::vector<DirEnt> list(FileError &ec, const Path &path) = 0;
		// Try to open a file in the filesystem.
		// The given path should already be in absolute form.
		virtual std::shared_ptr<FileDesc> open(FileError &ec, const Path &path, OpenMode mode) = 0;
		// Try to move a file from one path to another.
		// The given paths should already be in absolute form.
		virtual bool move(FileError &ec, const Path &source, const Path &dest) = 0;
		// Try to remove a file.
		// The given path should already be in absolute form.
		virtual bool remove(FileError &ec, const Path &path) = 0;
};



// Splits a path at every '/' character.
// Discards duplicates, the beginning and end.
void splitPath(std::vector<std::string> &out, std::string in);
// The opposite of `splitPath`!
template<typename Arr>
std::string mergePath(Arr parts, bool isRelative) {
	std::string out;
	
	for (auto s: parts) {
		if (out.length() || !isRelative) out += '/';
		out += s;
	}
	
	if (!out.length()) out = '/';
	
	return out;
}
// Turns a given path into an absolute path.
std::string absolutePath(std::string in);

// Tells whether a string is a valid path.
bool isValidPath(const std::string &in);
// Tells whether a string is a valid filename.
bool isValidFilename(const std::string &in);
// Tells whether a character is valid for filenames.
bool isValidFilename(char in);

// Create a file descriptor from a FileDesc object.
// Said file descriptor will become an owner of the FileDesc.
// A call to `fclose()` will call `close()` on the FileDesc.
FILE *createFD(std::shared_ptr<FileDesc> from);
// Set the filesystem to use for fopen, remove, etc.
void setFS(std::shared_ptr<Filesystem> from);

// Dumps some info about a file descriptor.
extern "C" void dumpinfo(FILE *fd);
