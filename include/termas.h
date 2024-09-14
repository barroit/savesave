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

/*
 * Extended termas functions for Windows
 */
#ifdef _WIN32

const char *strwinerror(void);

#define warn_winerr(...) \
	__warn_routine("warning: ", strwinerror(), __VA_ARGS__)

#define error_winerr(...) \
	__error_routine("error: ", strwinerror(), __VA_ARGS__)

#define die_winerr(...) __die_routine("fatal: ", strwinerror(), __VA_ARGS__)

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* TERMAS_H */
