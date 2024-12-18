/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * This file shall only be included by build system
 */

#ifndef BRTCOMCENT_H
#define BRTCOMCENT_H

#include "ansidecl.h"

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <zstd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>
#include <signal.h>
#include <threads.h>

#if defined(__unix__)
# include <unistd.h>
# include <pwd.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <libgen.h>
# include <dirent.h>
# include <sys/select.h>
# include <sys/resource.h>
# include <pthread.h>
# include <sys/utsname.h>
# include <sys/syscall.h>
# include <sys/mman.h>
# include <sys/wait.h>
#elif defined(_WIN32)
# include <sdkddkver.h>
# include <io.h>
# include <conio.h>
# include <malloc.h>
# include <winsock2.h>
# include <windows.h>
# include <windowsx.h>
# include <versionhelpers.h>
# include <commctrl.h>
# include <direct.h>
# include <basetsd.h>
# include <shlwapi.h>
# ifdef HAVE_RPHK
#  include <dbghelp.h>
#  include <crtdbg.h>
# endif
# include <process.h>
#endif

#ifdef __linux__
# include <linux/io_uring.h>
#endif

#include "compiler.h"
#include "poison.h"

#include "barroit/types.h"
#include "barroit/limits.h"
#include "barroit/ctype.h"
#include "barroit/size.h"

#include "barroit/string.h"
#include "barroit/time.h"
#include "barroit/resource.h"
#include "barroit/zstd.h"

#ifdef __linux__
# include <arch/syscall.h>
#endif

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTLDLL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# include <locale.h>
#endif

#include "i18n.h"

#if defined(__unix__)

#define MKDIR(path) mkdir(path, 0775)

#define NULL_DEVICE "/dev/null"

#elif defined(_WIN32)

char *strchrnul(const char *s, int c);

char *dirname(char *path);
char *basename(char *path);

int setenv(const char *name, const char *value, int overwrite);

void __noreturn __exit(int code);

int __unlink(const char *name);

#define read   _read
#define write  _write
#define strdup _strdup
#define access _access
#define fileno _fileno
#define mkdir  _mkdir
#define putenv _putenv
#define unlink __unlink
#define rmdir  _rmdir
#define getpid _getpid

#define exit __exit

#define MKDIR mkdir

/*
 * Microsoft documentation does not specify executable mode value, we better
 * define this to 00 instead of posix defined value 01
 */
#define X_OK 00
#define F_OK 00
#define W_OK 02
#define R_OK 04

#define _S_IFLNK 0x3939

#define S_IFREG _S_IFREG
#define S_IFLNK _S_IFLNK
#define S_IFDIR _S_IFDIR
#define S_IFMT  _S_IFMT

#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

#define STDIN_FILENO ({ fileno(stdin); })
#define STDOUT_FILENO ({ fileno(stdout); })
#define STDERR_FILENO ({ fileno(stderr); })

#define NULL_DEVICE "NUL"
#endif

#include "cntio.h"

#include "calc.h"

#endif /* BRTCOMCENT_H */
