
#include "devfs.hpp"

// List the files in a directory.
// The given path should already be in absolute form.
std::vector<DirEnt> DevFS::list(FileError &ec, const Path &path) {
	// FLAT DevFS.
	if (path != "/") {
		ec = FileError::NOT_FOUND;
		return std::vector<DirEnt>();
	}
	
	// Else!
	return std::vector<DirEnt>{
		DirEnt{"null", 0, 0, {1, 1, 0}, {1, 1, 0}, {1, 1, 0}, false, 0, 0}
	};
}

// Try to open a file in the filesystem.
// The given path should already be in absolute form.
std::shared_ptr<FileDesc> DevFS::open(FileError &ec, const Path &path, OpenMode mode) {
	if (path == "/null") {
		return std::make_shared<NullFile>();
	} else {
		ec = FileError::NOT_FOUND;
		return nullptr;
	}
}

// Try to move a file from one path to another.
// The given paths should already be in absolute form.
bool DevFS::move(FileError &ec, const Path &source, const Path &dest) {
	ec = FileError::NOT_SUPPORTED;
	return false;
}

// Try to remove a file.
// The given path should already be in absolute form.
bool DevFS::remove(FileError &ec, const Path &path) {
	ec = FileError::NOT_SUPPORTED;
	return false;
}
