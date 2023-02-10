
/*
    ABI definition - <stdio.h>
    
    This file includes a subset of the most commonly used functions from <stdio.h>
    For this reason, this file is subject to copyright as in <stdio.h> from gcc.
    
    See `LICENSE` for details.
*/

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque wrapper for FILE.
// typedef struct {} FILE;
// File I/O position type.
// typedef long fpos_t;

// Used to share reent pointers from sys/reent.h
void *__get_reent();
// Used to share reent pointers from sys/reent.h
void *__get_global_reent();

/* Functions included in ABI: */

// FILE *tmpfile();
// char *tmpnam ();
// FILE *fopen  (const char *restrict _name, const char *restrict _type);
// int  fclose  (FILE *);
// int  fflush  (FILE *);
// int  freopen (const char *restrict, const char *restrict, FILE *restrict);
// int  setvbuf (FILE *restrict, char *restrict, int, size_t);

// int  fprintf (FILE *restrict, const char *restrict, ...)
//                __attribute__ ((__format__ (__printf__, 2, 3)));
// int  fscanf  (FILE *restrict, const char *restrict, ...)
//                __attribute__ ((__format__ (__scanf__, 2, 3)));
// int  printf  (const char *restrict, ...)
//                __attribute__ ((__format__ (__printf__, 1, 2)));
// int  scanf   (const char *restrict, ...)
//                __attribute__ ((__format__ (__scanf__, 1, 2)));
// int  sscanf  (const char *restrict, const char *restrict, ...)
//                __attribute__ ((__format__ (__scanf__, 2, 3)));
// int  sprintf (char *restrict, const char *restrict, ...)
//                __attribute__ ((__format__ (__printf__, 2, 3)));
// int  snprintf(char *restrict, size_t, const char *restrict, ...)
//                __attribute__ ((__format__ (__printf__, 3, 4)));

// int   fgetc  (FILE *);
// char *fgets  (char *restrict, int, FILE *restrict);
// int   fputc  (int, FILE *);
// int   fputs  (const char *restrict, FILE *restrict);
// int   getc   (FILE *);
// int   getchar(void);
// char *gets   (char *);
// int   putc   (int, FILE *);
// int   putchar(int);
// int   puts   (const char *);
// int   ungetc (int, FILE *);

// size_t fread (void *restrict, size_t _size, size_t _n, FILE *restrict);
// size_t fwrite(const void *restrict , size_t _size, size_t _n, FILE *);

// int  fgetpos (FILE *restrict, fpos_t *restrict);
// int  fseek   (FILE *, long, int);
// int  fsetpos (FILE *, const fpos_t *);

// long ftell   (FILE *);
// void rewind  (FILE *);
// void clearerr(FILE *);
// int  feof    (FILE *);
// int  ferror  (FILE *);
// void perror  (const char *);

// int  remove  (const char *);
// int  rename  (const char *, const char *);

#ifdef __cplusplus
} // extern "C"
#endif
