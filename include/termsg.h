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

void __warn_routine(const char *pref, const char *extr,
		    const char *fmt, ...) FORMAT(3, 4);

int __error_routine(const char *pref, const char *extr,
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

#ifdef NDEBUG
# define BUG_ON(cond) do { if (cond) {} } while (0)
#else
# define BUG_ON(cond) assert(!(cond))
#endif

#define EXIT_ON(cond) do { if (cond) exit(128); } while (0)

int redirect_output(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* TERMSG_H */

