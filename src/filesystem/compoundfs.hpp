
#pragma once

#include "customio.hpp"

// Mount point entry.
struct Mount {
	// Mount location.
	Path path;
	// Subfilesystem object.
	std::shared_ptr<Filesystem> fs;
};

class CompoundFS: public Filesystem {
	protected:
		// List of mount points.
		// Mounted filesystems get priority over the files within.
		std::vector<Mount> mounts;
		// Get an iterator for mount point, or end() if not present.
		// Path must be the exact mount path.
		std::vector<Mount>::iterator findMountExact(const Path &path);
		// Get an iterator for mount point, or end() if not present.
		// Mount shall be the mounted filesystem under which path lies.
		std::vector<Mount>::iterator findMount(const Path &path);
		
	public:
		// Mount a filesystem at the given path.
		// The given path must be absolute.
		// Returns false if there is already a mounted filesystem there.
		bool mount(const Path &path, std::shared_ptr<Filesystem> fs);
		// Remove the filesystem at the given path.
		// Returns true if there was a mounted filesystem removed.
		bool unmount(const Path &path);
		// Obtain a copy of the mounts.
		const std::vector<Mount> &mounted() const { return mounts; }
		// Get the filesystem mounted at the given path.
		// Only returns filesystems with that exact mount point.
		std::shared_ptr<Filesystem> mounted(const Path &path);
		
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
};
