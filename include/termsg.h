/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef TERMSG_H
#define TERMSG_H

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

#define warn(...) __warn_routine("warning: ", 0, __VA_ARGS__)
#define warn_errno(...) \
	__warn_routine("warning: ", strerror(errno), __VA_ARGS__)

#define error(...) __error_routine("error: ", 0, __VA_ARGS__)
#define error_errno(...) \
	__error_routine("error: ", strerror(errno), __VA_ARGS__)

#define die(...) __die_routine("fatal: ", 0, __VA_ARGS__)
#define die_errno(...) \
	__die_routine("fatal: ", strerror(errno), __VA_ARGS__)

#define bug(...) bug_routine(__FILE__, __LINE__, __VA_ARGS__)

int redirect_stdio(const char *filename);

/* error messages */
#define ERR_OPEN_FILE "failed to open file ‘%s’"
#define ERR_OPEN_DIR  "failed to open directory ‘%s’"

#define ERR_STAT_FILE "failed to retrieve information for file ‘%s’"

#define ERR_ACCESS_FILE "failed to access ‘%s’"

#ifdef __cplusplus
}
#endif

#endif /* TERMSG_H */
