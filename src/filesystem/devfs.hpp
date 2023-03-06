
#pragma once

#include "customio.hpp"

class NullFile: public FileDesc {
	public:
		NullFile(OpenMode mode = OpenMode{true, true, false, true}):
			FileDesc(mode) {}
		
		// Read bytes from this file.
		// Returns read length.
		int read(FileError &ec, char *out, int len) { return 0; }
		
		// Write bytes to this file.
		// Returns written length.
		int write(FileError &ec, const char *in, int len) {
			fwrite(in, 1, len, stdout);
			return len;
		}
		
		// Seeks in the file.
		// Returns new position on success, -1 on error.
		int seek(FileError &ec, _fpos_t off, int whence) { return 0; }
		
		// Closes the file.
		// Returns 0 on success, -1 on error.
		int close(FileError &ec) { open = false; return 0; }
		
		// Gets the absolute position in the file.
		long tell() { return 0; }
};

class DevFS: public Filesystem {
	public:
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
		bool sync(FileError &ec) { return true; }
};
