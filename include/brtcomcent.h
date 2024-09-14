/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * This file shall only be included by build system
 */

#ifndef BRTCOMCENT_H
#define BRTCOMCENT_H

/*
 * include MSVC's <atomic> with stdatomic.h provided by clang, will cause
 * function signature conflicting
 * 
 * clang --- void atomic_thread_fence(memory_order)
 * conflicts
 * MSVC  --- _EXPORT_STD extern "C" inline void atomic_thread_fence(const
 *           memory_order _Order) noexcept
 */
#ifdef _WIN32
# define _ATOMIC_
#endif

#ifdef __cplusplus
# include <new>
# include <iomanip>
# include <sstream>
# include <iostream>
# include <string>
#endif

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
#include <stdatomic.h>

#ifdef _WIN32
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
# ifndef NDEBUG
#  include <dbghelp.h>
#  include <crtdbg.h>
# endif
#else /* linux */
# include <unistd.h>
# include <pwd.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <libgen.h>
# include <dirent.h>
# include <sys/select.h>
#endif

#include "compiler.h"
#include "poison.h"

#include "ansidecl.h"
#include "poll.h"
#include "getopt.h"

#include "barroit/ctype.h"
#include "barroit/limits.h"
#include "barroit/size.h"
#include "barroit/string.h"
#include "barroit/types.h"
#include "barroit/zstd.h"

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTLDLL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# include <locale.h>
#endif

#include "i18n.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

#define STDOUT_FILENO ({ fileno(stdout); })
#define STDERR_FILENO ({ fileno(stderr); })

#define read   _read
#define write  _write
#define strdup _strdup
#define access _access
#define fileno _fileno
#define mkdir  _mkdir
#define putenv _putenv

#define F_OK 00
#define W_OK 02
#define R_OK 04
/*
 * Microsoft documentation does not specify executable mode value, we better
 * define this to 00 instead of posix defined value 01
 */
#define X_OK 00

#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)

#define SSIZE_MAX INT64_MAX
typedef SSIZE_T ssize_t;

#define PATH_MAX MAX_PATH

typedef unsigned int mode_t;

typedef off_t off64_t;

char *strchrnul(const char *s, int c);
char *dirname(char *path);
int setenv(const char *name, const char *value, int overwrite);

NORETURN winexit(int code);
#define exit winexit

#define MKDIR mkdir

#else /* ---- linux ----- */

#define MKDIR(path) mkdir(path, 0775)

#endif /* ---- _WIN32 ----- */

#ifdef __cplusplus
}
#endif

#include "cntio.h"

#endif /* BRTCOMCENT_H */
