
#ifndef _DIRENT_H_
# error "Never use <sys/dirent.h> directly; include <dirent.h> instead."
#endif

// Provide `struct dirent` type.
struct dirent {
	__ino_t d_ino;
	__off_t d_off;
	unsigned short int d_reclen;
	unsigned char d_type;
	char d_name[256];
};

// For backwards compatibility.
#define d_fileno d_ino

#undef  _DIRENT_HAVE_D_NAMLEN
#define _DIRENT_HAVE_D_RECLEN
#define _DIRENT_HAVE_D_OFF
#define _DIRENT_HAVE_D_TYPE

#define _DIRENT_MATCHES_DIRENT64 0
