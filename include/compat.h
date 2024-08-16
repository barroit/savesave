/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * This file shall only be include by build system
 */

#ifndef COMPAT_H
#define COMPAT_H

#ifdef __cplusplus
# include <new>
# include <iomanip>
# include <sstream>
# include <iostream>
# include <string>
# include <filesystem>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <zip.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif

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
#else /* linux */
# include <unistd.h>
# include <pwd.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <libgen.h>
# include <dirent.h>
# ifndef NO_SYS_SELECT_H
#  include <sys/select.h>
# endif
#endif

#include "compiler.h"

#include "generated/appname.h"
#include "generated/version.h"
#include "generated/apphelp.h"

#ifdef _WIN32
NORETURN winexit(int code);
# define exit winexit
#endif

#if (defined __clang__ && defined _WIN32)
# define WINDOWS_NATIVE
#endif

#ifdef WINDOWS_NATIVE
# include <BaseTsd.h>
# include <stdint.h>
# define SSIZE_MAX INT64_MAX
typedef SSIZE_T ssize_t;
#endif

#ifdef WINDOWS_NATIVE
# define read	_read
# define write	_write
# define open	_open
# define close	_close
# define strdup	_strdup
# define access	_access
# define fileno	_fileno
# define dup2	_dup2
# define mkdir  _mkdir
#endif

#ifdef _WIN32
char *strchrnul(const char *s, int c);
#endif

#ifdef _WIN32
# define my_mkdir    mkdir
#else
# define my_mkdir(p) mkdir(p, 0775)
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPAT_H */
