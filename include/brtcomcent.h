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
# include <process.h>
#else /* linux */
# include <unistd.h>
# include <pwd.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <libgen.h>
# include <dirent.h>
# include <sys/select.h>
# include <sys/resource.h>
#endif

#include "ansidecl.h"
#include "compiler.h"
#include "poison.h"

#include "poll.h"

#include "barroit/types.h"
#include "barroit/limits.h"
#include "barroit/ctype.h"
#include "barroit/size.h"

#include "barroit/string.h"
#include "barroit/time.h"
#include "barroit/resource.h"
#include "barroit/threads.h"
#include "barroit/zstd.h"

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTLDLL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# include <locale.h>
#endif

#include "i18n.h"

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
#define unlink _unlink
#define rmdir  _rmdir
#define getpid _getpid

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

typedef unsigned int uint;
typedef unsigned short ushort;

typedef uint mode_t;

typedef off_t off64_t;

typedef int pid_t;

char *strchrnul(const char *s, int c);
char *dirname(char *path);
int setenv(const char *name, const char *value, int overwrite);

#ifndef CONFIG_IS_CONSOLE_APP
NORETURN winexit(int code);
# define exit winexit
#endif

#define MKDIR mkdir

#else /* ---- linux ----- */

#define MKDIR(path) mkdir(path, 0775)

#endif /* ---- _WIN32 ----- */

#include "cntio.h"

#endif /* BRTCOMCENT_H */
