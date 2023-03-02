
#include "compoundfs.hpp"

// Get an iterator for mount point, or end() if not present.
// Path must be the exact mount path.
std::vector<Mount>::iterator CompoundFS::findMountExact(const Path &path) {
	printf("findMountExact(\"%s\")", path.string().c_str());
	std::vector<Mount>::iterator iter = mounts.begin();
	for (; iter != mounts.end(); iter++) {
		if (iter->path == path) return iter;
	}
	if (iter == mounts.end()) printf(": Not found\n");
	else printf(": mount at %s\n", iter->path.string().c_str());
	return iter;
}

// Get an iterator for mount point, or end() if not present.
// Mount shall be the mounted filesystem under which path lies.
std::vector<Mount>::iterator CompoundFS::findMount(const Path &path) {
	printf("findMount(\"%s\")", path.string().c_str());
	// Current longest match.
	std::vector<Mount>::iterator max = mounts.end();
	
	// Search through all mounts.
	for (auto iter = mounts.begin(); iter != mounts.end(); iter++) {
		if (path.startsWith(iter->path)) {
			// Max is absent or shorter than current match?
			if (max == mounts.end() || max->path.parts().size() < iter->path.parts().size()) {
				max = iter;
			}
		}
	}
	
	// Return findings.
	if (max == mounts.end()) printf(": Not found\n");
	else printf(": mount at %s\n", max->path.string().c_str());
	return max;
}


// Mount a filesystem at the given path.
// The given path must be absolute.
// Returns false if there is already a mounted filesystem there.
bool CompoundFS::mount(const Path &path, std::shared_ptr<Filesystem> fs) {
	if (mounted(path)) return false;
	mounts.push_back(Mount{path, fs});
	return true;
}

// Remove the filesystem at the given path.
// Returns true if there was a mounted filesystem removed.
bool CompoundFS::unmount(const Path &path) {
	auto iter = findMountExact(path);
	if (iter != mounts.end()) {
		mounts.erase(iter);
	}
	return iter != mounts.end();
}

// Get the filesystem mounted at the given path.
// Only returns filesystems with that exact mount point.
std::shared_ptr<Filesystem> CompoundFS::mounted(const Path &path) {
	auto iter = findMountExact(path);
	if (iter != mounts.end()) {
		return iter->fs;
	}
	return nullptr;
}


// List the files in a directory.
// The given path should already be in absolute form.
std::vector<DirEnt> CompoundFS::list(FileError &ec, const Path &path) {
	// Find the subject filesystem.
	auto mount = findMount(path);
	
	// If found, delegate.
	if (mount != mounts.end()) {
		return mount->fs->list(ec, path.substr(mount->path.parts().size()));
	} else {
		ec = FileError::NOT_FOUND;
		return std::vector<DirEnt>();
	}
}

// Try to open a file in the filesystem.
// The given path should already be in absolute form.
std::shared_ptr<FileDesc> CompoundFS::open(FileError &ec, const Path &path, OpenMode mode) {
	// Find the subject filesystem.
	auto mount = findMount(path);
	
	// If found, delegate.
	if (mount != mounts.end()) {
		return mount->fs->open(ec, path.substr(mount->path.parts().size()), mode);
	} else {
		ec = FileError::NOT_FOUND;
		return nullptr;
	}
}

// Try to move a file from one path to another.
// The given paths should already be in absolute form.
bool CompoundFS::move(FileError &ec, const Path &source, const Path &dest) {
	// Find the subject filesystems.
	auto sourceMount = findMount(source);
	auto destMount = findMount(dest);
	
	// Not found, one of them.
	if (sourceMount == mounts.end() || destMount == mounts.end()) {
		ec = FileError::NOT_FOUND;
		return false;
	}
	
	// TODO: Not the same filesystem, perform a copy.
	if (sourceMount != destMount) {
		ec = FileError::NOT_SUPPORTED;
		return false;
	}
	
	// Move within the same filesystem, delegate.
	return sourceMount->fs->move(ec,
		source.substr(sourceMount->path.parts().size()),
		dest.substr(destMount->path.parts().size())
	);
}

// Try to remove a file.
// The given path should already be in absolute form.
bool CompoundFS::remove(FileError &ec, const Path &path) {
	// Find the subject filesystem.
	auto mount = findMount(path);
	
	// If found, delegate.
	if (mount != mounts.end()) {
		return mount->fs->remove(ec, path.substr(mount->path.parts().size()));
	} else {
		ec = FileError::NOT_FOUND;
		return false;
	}
}