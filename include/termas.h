/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef TERMAS_H
#define TERMAS_H

#ifdef __cplusplus
extern "C" {
#endif

int __warn_routine(const char *pref, const char *extr,
		   const char *fmt, ...) FORMAT(3, 4);

int __error_routine(const char *pref, const char *extr,
		    const char *fmt, ...) FORMAT(3, 4);

NORETURN __die_routine(const char *pref, const char *extr,
		       const char *fmt, ...) FORMAT(3, 4);

NORETURN bug_routine(const char *file, int line,
		     const char *fmt, ...) FORMAT(3, 4);

#define warn(...) \
	__warn_routine(_("warning: "), 0, __VA_ARGS__)
#define warn_errno(...) \
	__warn_routine(_("warning: "), strerror(errno), __VA_ARGS__)

#define error(...) \
	__error_routine(_("error: "), 0, __VA_ARGS__)
#define error_errno(...) \
	__error_routine(_("error: "), strerror(errno), __VA_ARGS__)

#define die(...) \
	__die_routine(_("fatal: "), 0, __VA_ARGS__)
#define die_errno(...) \
	__die_routine(_("fatal: "), strerror(errno), __VA_ARGS__)

#define bug(...) bug_routine(__FILE__, __LINE__, __VA_ARGS__)

/* common error messages */
#define ERRMAS_OPEN_FILE   "failed to open file ‘%s’"
#define ERRMAS_CREAT_FILE  "failed to create file at ‘%s’"
#define ERRMAS_STAT_FILE   "failed to retrieve information for file ‘%s’"
#define ERRMAS_ACCESS_FILE "failed to access file ‘%s’"

#ifdef __cplusplus
}
#endif

#endif /* TERMAS_H */
