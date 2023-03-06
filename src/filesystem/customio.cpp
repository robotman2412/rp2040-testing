
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "customio.hpp"

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <filesystem>

// A list of all living FileDesc wrappers created.
static std::vector<std::shared_ptr<FileDesc>> fileWrappers;

// The current filesystem to use.
static std::shared_ptr<Filesystem> filesystem;

// The current working directory.
static std::string cwd = "/home/user";



// Wrapper for FileDesc::read.
static int read_wrapper(struct _reent *reent, void *cookie, char *out, int len) {
	FileError ec = FileError::OK;
	// Get the FileDesc object.
	auto desc = (FileDesc *) cookie;
	// Forward the function call.
	int res = desc->read(ec, out, len);
	if (res) errno = (int) ec;
	return res;
}

// Wrapper for FileDesc::write.
static int write_wrapper(struct _reent *reent, void *cookie, const char *in, int len) {
	FileError ec = FileError::OK;
	// Get the FileDesc object.
	auto desc = (FileDesc *) cookie;
	// Forward the function call.
	int res = desc->write(ec, in, len);
	if (res) errno = (int) ec;
	return res;
}

// Wrapper for FileDesc::seek.
static _fpos_t seek_wrapper(struct _reent *reent, void *cookie, _fpos_t off, int whence) {
	FileError ec = FileError::OK;
	// Get the FileDesc object.
	auto desc = (FileDesc *) cookie;
	// Forward the function call.
	return desc->seek(ec, off, whence);
}

// Wrapper for FileDesc::close.
static int close_wrapper(struct _reent *reent, void *cookie) {
	FileError ec = FileError::OK;
	// Get the FileDesc object.
	auto desc = (FileDesc *) cookie;
	// Call its close first.
	int res = desc->close(ec);
	
	// Erase it from the list.
	for (auto iter = fileWrappers.begin(); iter != fileWrappers.end(); ++iter) {
		if (iter->get() == desc) {
			fileWrappers.erase(iter);
			break;
		}
	}
	
	// Return the earlier result.
	if (res) errno = (int) ec;
	return res;
}



// Parse from a string.
OpenMode OpenMode::parse(std::string in) {
	bool plus = false;
	OpenMode out{0,0,0,0};
	for (char c: in) {
		if (c == 'r') {
			out.read = true;
		} else if (c == 'w') {
			out.write = true;
		} else if (c == 'a') {
			out.append = true;
		} else if (c == '+') {
			plus = true;
		} else if (c == 'b') {
			out.binary = true;
		}
	}
	if (plus) {
		out.read = true;
		out.write = true;
	}
	return out;
}


// Make from string.
Path::Path(const std::string &in): str(in) {
	splitPath(_parts, in);
	_relative = in[0] != '/';
}

// Make from list of parts.
Path::Path(const std::vector<std::string> &in, bool relative):
	_parts(in), str(mergePath(in, relative)), _relative(relative) {}

// Set from string representation.
void Path::string(std::string in) {
	_parts.clear();
	str = in;
	splitPath(_parts, in);
	_relative = in[0] != '/';
}

// Set from parts representation.
void Path::parts(std::vector<std::string> newValue) {
	_parts = std::move(newValue);
	str = mergePath(_parts, _relative);
}

// Set relative.
void Path::relative(bool newValue) {
	_relative = newValue;
	if (_relative && str[0] != '/') {
		str = '/' + str;
	} else if (!_relative && str[0] == '/') {
		str = str.substr(1);
	}
}

// Test whether this path starts with another.
bool Path::startsWith(const Path &other) const {
	if (_parts.size() < other._parts.size()) return false;
	for (std::size_t i = 0; i < other._parts.size(); i++) {
		if (_parts[i] != other._parts[i]) return false;
	}
	return true;
}

// Concatenate to another path.
Path Path::operator+(const Path &other) const {
	Path out(*this);
	for (auto s: other._parts) {
		out._parts.push_back(s);
	}
	out.str = mergePath(out._parts, _relative);
	return out;
}

// Get a sub-path.
Path Path::substr(std::size_t start, std::size_t len) const {
	if (!len || start >= _parts.size()) return Path(".");
	if (len + start > _parts.size()) len = _parts.size() - start;
	
	std::vector<std::string> new_parts;
	
	new_parts.assign(_parts.begin() + start, _parts.begin() + start + len);
	
	return Path(new_parts, true);
}



// Splits a path at every '/' character.
// Discards duplicates, the beginning and end.
void splitPath(std::vector<std::string> &out, std::string in) {
	if (in.length() == 0) return;
	
	// First non-slash character index.
	int first = -1;
	// Currently inspecting index.
	int i = 0;
	while (i < in.length()) {
		// Check a character.
		if (in[i] == '/') {
			if (first != -1) out.push_back(in.substr(first, i-first));
			first = -1;
		} else if (first == -1) {
			first = i;
		}
		
		// Next!
		i++;
	}
	// Check for trailing things.
	if (first != -1) out.push_back(in.substr(first, i-first));
}

// Turns a given path into an absolute path.
std::string absolutePath(std::string in) {
	if (in.size() == 0) {
		return cwd;
	}
	
	// This shall be our path thing.
	std::vector<std::string> parts;
	
	// Add current working dir to path.
	if (in[0] != '/') {
		splitPath(parts, cwd);
	}
	
	// Add relative part to path.
	splitPath(parts, in);
	
	// Time for reduction.
	for (int i = 0; i < parts.size();) {
		if (parts[i] == ".") {
			// Current directory? Remove.
			parts.erase(parts.begin() + i);
			
		} else if (parts[i] == "..") {
			// Directory up? Remove part before this.
			parts.erase(parts.begin() + i);
			if (i > 0) {
				parts.erase(parts.begin() + i - 1);
				i--;
			}
		} else {
			// Something else? Keep it.
			i++;
		}
	}
	
	// Count length requirement.
	size_t len = 0;
	for (std::string s: parts) {
		len += 1 + s.length();
	}
	std::string out;
	out.reserve(len);
	
	// Start concatenating.
	for (std::string s: parts) {
		out += '/';
		out += s;
	}
	
	// If empty, put /.
	if (!out.length()) out = '/';
	
	return out;
}



// Tells whether a string is a valid path.
bool isValidPath(const std::string &in) {
	Path path(in);
	for (const std::string &s: path.parts()) {
		if (!isValidFilename(s) && s != "." && s != "..") return false;
	}
	return true;
}

// Tells whether a string is a valid filename.
bool isValidFilename(const std::string &in) {
	for (char c: in) {
		if (!isValidFilename(c)) return false;
	}
	if (in == "." || in == "..") return false;
	return true;
}

// Tells whether a character is valid for filenames.
bool isValidFilename(char in) {
	return in && in != '/';
}



// Create a file descriptor from a FileDesc object.
// Said file descriptor will become an owner of the FileDesc.
// A call to `fclose()` will call `close()` on the FileDesc.
FILE *createFD(std::shared_ptr<FileDesc> from) {
	if (!from.get()) return NULL;
	FILE *out = new FILE;
	
	// Take ownership of the file object.
	fileWrappers.push_back(from);
	
	// Generic __sFILE things.
	out->_p = nullptr;
	out->_r = 0;
	out->_w = 0;
	
	// A bunch of flags.
	out->_flags = __SNBF;
	if (from->isReadWrite()) out->_flags |= __SRW;
	else if (from->isWrite()) out->_flags |= __SWR;
	else /* from->isRead() */ out->_flags |= __SRD;
	
	// More generic __sFILE things.
	out->_file = -1;
	out->_bf = { nullptr, 0 };
	out->_lbfsize = 0;
	
	// Accessor functions.
	out->_cookie = from.get();
	out->_read = read_wrapper;
	out->_write = write_wrapper;
	out->_seek = seek_wrapper;
	out->_close = close_wrapper;
	
	// More generic __sFILE things.
	out->_ub = { nullptr, 0 };
	out->_up = nullptr;
	out->_ur = 0;
	out->_lb = { nullptr, 0 };
	out->_blksize = 0;
	out->_offset = 0;
	out->_mbstate = { 0, { .__wch = 0 } };
	out->_flags2 = 0;
	
	return out;
}

// Set the filesystem to use for fopen, remove, etc.
void setFS(std::shared_ptr<Filesystem> from) {
	filesystem = from;
}



// Provide an implementation of pathconf.
long pathconf(const char *_path, int name) {
	switch (name) {
		case _PC_LINK_MAX:  return 1;
		case _PC_MAX_CANON: return 2048;
		case _PC_MAX_INPUT: return 2048;
		case _PC_NAME_MAX:  return 16;
		case _PC_PATH_MAX:  return 2048;
		case _PC_PIPE_BUF:  return 2048;
		case _PC_CHOWN_RESTRICTED: return 1;
		case _PC_NO_TRUNC:  return 1;
		case _PC_VDISABLE:  return 1;
		default:
			errno = EINVAL;
			return -1;
	}
}

// Provide an implementation of fopen.
FILE *fopen(const char *path, const char *mode) {
	if (!filesystem) return nullptr;
	
	FileError ec = FileError::OK;
	auto ptr = filesystem->open(ec, path, OpenMode::parse(mode));
	if (!ptr) {
		errno = (int) ec;
		return nullptr;
	}
	
	return createFD(ptr);
}

// Provide an implementation of getcwd.
char *getcwd(char *buf, size_t size) {
	if (!buf) {
		return strdup(cwd.c_str());
	} else if (size) {
		memcpy(buf, cwd.c_str(), size-1);
		buf[size-1] = 0;
		return buf;
	} else {
		return buf;
	}
}

// Provide an implementation of chdir.
int chdir(const char *path) {
	cwd = absolutePath(path);
	return 0;
}



// Dumps some info about a file descriptor.
extern "C" void dumpinfo(FILE *fd) {
	if (!fd) return;
	printf("struct __sFILE info:\n");
	printf("  _p = 0x%p,\n", fd->_p);
	printf("  _r = %d,\n", fd->_r);
	printf("  _w = %d,\n", fd->_w);
	printf("  _flags = 0x%x,\n", fd->_flags);
	printf("  _file = %d,\n", fd->_file);
	printf("  _bf = { 0x%p, %d },\n", fd->_bf._base, fd->_bf._size);
	printf("  _lbfsize = %d,\n", fd->_lbfsize);
	printf("  _cookie = 0x%p,\n", fd->_cookie);
	printf("  _read = 0x%p,\n", fd->_read);
	printf("  _write = 0x%p,\n", fd->_write);
	printf("  _seek = 0x%p,\n", fd->_seek);
	printf("  _close = 0x%p,\n", fd->_close);
	printf("  _ub = { 0x%p, %d },\n", fd->_ub._base, fd->_ub._size);
	printf("  _up = 0x%p,\n", fd->_up);
	printf("  _ur = %d,\n", fd->_ur);
	printf("  _lb = { 0x%p, %d },\n", fd->_lb._base, fd->_lb._size);
	printf("  _blksize = %d,\n", fd->_blksize);
	printf("  _offset = %d,\n", fd->_offset);
}
