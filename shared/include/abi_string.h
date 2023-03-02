
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Find the first occurrance of `__c` in `__mem`, searching `__len` bytes.
// Returns a pointer if found, NULL if not.
void *memchr(const void *__mem, int __c, size_t __len);
// Compare `__a` against `__b`, searching `__len` bytes.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int   memcmp(const void *__a, const void *__b, size_t __len);
// Copy `__len` bytes from `__src` to `__dst`.
// Returns `__dst`.
void *memcpy(void *restrict __dst, const void *restrict __src, size_t __len);
// Move `__len` bytes from `__src` to `__dst`.
// Promises to correctly move data when `__src` and `__dst` overlap.
// Returns `__dst`.
void *memmove(void *__dst, const void *__src, size_t __len);

// Determine length of `__src`, search at most `__len` characters.
// Returns length excluding null terminator.
size_t strnlen(const char *__src, size_t __len);
// Concatenate at most `__len` characters of `__src` onto `__dst`.
// Returns `__dst`.
char *strncat(char *restrict __dst, const char *restrict __src, size_t __len);
// Find the first occurrance of `__c` in `__src`, searching at most `__len` characters.
// Returns a pointer if found, NULL if not.
char *strnchr(const char *__src, int __c, size_t __len);
// Find the last occurrance of `__c` in `__src`, searching at most `__len` characters.
// Returns a pointer if found, NULL if not.
char *strnrchr(const char *__src, int __c, size_t __len);
// Compare `__a` against `__b`, searching at most `__len` characters.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int   strncmp(const char *__a, const char *__b, size_t __len);
// [Alias to strncmp]
// Compare `__a` against `__b`, searching at most `__len` characters.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int   strncoll(const char *__a, const char *__b, size_t __len);
// [UNSAFE FUNCTION: use strncat instead]
// Copy at most `__len` characters from `__src` into `__dst`.
// WARNING: Always sets exactly `__len` bytes of `__dst`, which means no null terminator if `__len` have already been copied.
// Returns `__dst`.
char *strncpy(char *restrict __dst, const char *restrict __src, size_t __len);
// Create a duplicate of at most `__len` characters from `__src`.
// Returns newly allocated memory with the copy.
char *strndup(const char *__src, size_t __len);

// [UNSAFE FUNCTION: use strnlen instead]
// Determine length of `__src`.
// Returns length excluding null terminator.
size_t strlen(const char *__src);
// [UNSAFE FUNCTION: use strncat instead]
// Concatenate `__src` onto `__dst`.
// Returns `__dst`.
char *strcat(char *restrict __dst, const char *restrict __src);
// [UNSAFE FUNCTION: use strnchr instead]
// Find the first occurrance of `__c` in `__src`.
// Returns a pointer if found, NULL if not.
char *strchr(const char *__src, int __c);
// [UNSAFE FUNCTION: use strnrchr instead]
// Find the last occurrance of `__c` in `__src`.
// Returns a pointer if found, NULL if not.
char *strrchr(const char *__src, int __c);
// [UNSAFE FUNCTION: use strncmp instead]
// Compare `__a` against `__b`.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int   strcmp(const char *__a, const char *__b);
// [UNSAFE FUNCTION: use strncmp instead]
// [Alias to strcmp]
// Compare `__a` against `__b`.
// Returns >0, 0, <0 when a>b, a==b, a<b respectively.
int   strcoll(const char *__a, const char *__b);
// [UNSAFE FUNCTION: use strncat instead]
// Copy from `__src` into `__dst`.
// Returns `__dst`.
char *strcpy(char *restrict __dst, const char *restrict __src);
// [UNSAFE FUNCTION: use strndup instead]
// Create a duplicate of `__src`.
// Returns newly allocated memory with the copy.
char *strdup(const char *__src);

#ifdef __cplusplus
}
#endif
