
#include "abi_string.h"
#include "abi_malloc.h"

#define STR_SIZE_MAX (unsigned) (1<<30)


// Find the first occurrance of `__c` in `__mem`, searching `__len` bytes.
// Returns a pointer if found, NULL if not.
void *memchr(const void *__mem, int __c, size_t __len) {
	for (size_t i = 0; i < __len; i++) {
		if (*(const char *) __mem == (char) __c) {
			return (void *) ((size_t) __mem + i);
		}
	}
	return NULL;
}

// Compare `__a` against `__b`, searching `__len` bytes.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int memcmp(const void *__a, const void *__b, size_t __len) {
	for (size_t i = 0; i < __len; i++) {
		int diff = ((const char *) __a)[i] - ((const char *) __b)[i];
		if (diff) return diff;
	}
	return 0;
}

// Copy `__len` bytes from `__src` to `__dst`.
// Returns `__dst`.
void *memcpy(void *restrict __dst, const void *restrict __src, size_t __len) {
	// This is a reverse copy impl.
	while (__len) {
		--__len;
		((char *) __dst)[__len] = ((const char *) __src)[__len];
	}
	return __dst;
}

// Move `__len` bytes from `__src` to `__dst`.
// Promises to correctly move data when `__src` and `__dst` overlap.
// Returns `__dst`.
void *memmove(void *__dst, const void *__src, size_t __len) {
	if (__dst <= __src) {
		// Hand off all these reverse copyable.
		return memcpy(__dst, __src, __len);
	} else {
		// Do our own forward copy.
		for (size_t i = 0; i < __len; i++) {
			((char *) __dst)[i] = ((const char *) __src)[i];
		}
		return __dst;
	}
}



// Determine length of `__src`, search at most `__len` characters.
// Returns length excluding null terminator.
size_t strnlen(const char *__src, size_t __len) {
	size_t i;
	for (i = 0; i < __len; i++) {
		if (!__src[i]) return i;
	}
	return __len;
}

// Concatenate at most `__len` characters of `__src` onto `__dst`.
// Returns `__dst`.
char *strncat(char *restrict __dst, const char *restrict __src, size_t __len) {
	// Compute offset to write at.
	size_t off = 0;
	while (__dst[off]) off++;
	
	// Concatenate.
	size_t i;
	for (i = 0; i < __len; i++) {
		if (!(__dst[off+i] = __src[i])) return __dst;
	}
	
	// Add missing null terminator.
	__dst[off+i] = 0;
	return __dst;
}

// Find the first occurrance of `__c` in `__src`, searching at most `__len` characters.
// Returns a pointer if found, NULL if not.
char *strnchr(const char *__src, int __c, size_t __len) {
	for (size_t i = 0; i < __len; i++) {
		const char next = __src[i];
		if (next == __c) return (char *) __src + i;
		else if (!next) return NULL;
	}
	
	return NULL;
}

// Find the last occurrance of `__c` in `__src`, searching at most `__len` characters.
// Returns a pointer if found, NULL if not.
char *strnrchr(const char *__src, int __c, size_t __len) {
	char *found = NULL;
	
	for (size_t i = 0; i < __len; i++) {
		if (__src[i] == __c) found = (char *) __src + i;
	}
	
	return found;
}

// Compare `__a` against `__b`, searching at most `__len` characters.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int strncmp(const char *__a, const char *__b, size_t __len) {
	for (size_t i = 0; i < __len; i++) {
		// This will be the comparison.
		int res = __a[i] - __b[i];
		// If not match (or different length) return found.
		if (res) return res;
		// If it does match and they end, return 0.
		else if (!__a[i]) return 0;
	}
	// Ran out of chars, so it is a match.
	return 0;
}

// [Alias to strncmp] Compare `__a` against `__b`, searching at most `__len` characters.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int strncoll(const char *__a, const char *__b, size_t __len) {
	return strncmp(__a, __b, __len);
}

// [UNSAFE FUNCTION] Copy at most `__len` characters from `__src` into `__dst`.
// WARNING: Always sets exactly `__len` bytes of `__dst`, which means no null terminator if `__len` have already been copied.
// Returns `__dst`.
char *strncpy(char *restrict __dst, const char *restrict __src, size_t __len) {
	size_t i;
	
	for (i = 0; i < __len; i++) {
		if (!(__dst[i] = __src[i])) break;
	}
	for (; i < __len; i++) {
		__dst[i] = 0;
	}
	
	return __dst;
}

// Create a duplicate of at most `__len` characters from `__src`.
// Returns newly allocated memory with the copy.
char *strndup(const char *__src, size_t __len) {
	__len = strnlen(__src, __len);
	char *mem = malloc(__len + 1);
	if (mem) {
		memcpy(mem, __src, __len);
		mem[__len] = 0;
	}
	return mem;
}



// [UNSAFE FUNCTION] Determine length of `__src`.
// Returns length excluding null terminator.
size_t strlen(const char *__src) {
	return strnlen(__src, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Concatenate `__src` onto `__dst`.
// Returns `__dst`.
char *strcat(char *restrict __dst, const char *restrict __src) {
	return strncat(__dst, __src, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Find the first occurrance of `__c` in `__src`.
// Returns a pointer if found, NULL if not.
char *strchr(const char *__src, int __c) {
	return strnchr(__src, __c, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Find the last occurrance of `__c` in `__src`, searching at most `__len` characters.
// Returns a pointer if found, NULL if not.
char *strrchr(const char *__src, int __c) {
	return strnrchr(__src, __c, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Compare `__a` against `__b`.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int strcmp(const char *__a, const char *__b) {
	return strncmp(__a, __b, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] [Alias to strcmp] Compare `__a` against `__b`.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int strcoll(const char *__a, const char *__b) {
	return strncmp(__a, __b, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Copy from `__src` into `__dst`.
// Returns `__dst`.
char *strcpy(char *restrict __dst, const char *restrict __src) {
	return strncpy(__dst, __src, STR_SIZE_MAX);
}

// [UNSAFE FUNCTION] Create a duplicate of `__src`.
// Returns newly allocated memory with the copy.
char *strdup(const char *__src) {
	return strndup(__src, STR_SIZE_MAX);
}


