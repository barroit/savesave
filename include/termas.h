/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef TERMAS_H
#define TERMAS_H

int __warn_routine(const char *prf,
		   const char *ext,
		   const char *fmt, ...) __format(3, 4);

int __error_routine(const char *prf,
		    const char *ext,
		    const char *fmt, ...) __format(3, 4);

void __noreturn __die_routine(const char *prf,
			      const char *ext,
			      const char *fmt, ...) __format(3, 4);

void __noreturn bug_routine(const char *file,
			    int line,
			    const char *fmt, ...) __format(3, 4);

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

int last_errno_error(void);

#define warn_winerr(...) \
	__warn_routine("warning: ", strwinerror(), __VA_ARGS__)

#define error_winerr(...) \
	__error_routine("error: ", strwinerror(), __VA_ARGS__)

#define die_winerr(...) \
	__die_routine("fatal: ", strwinerror(), __VA_ARGS__)

#endif /* _WIN32 */

int termas_rd_output(const char *name);

/*
 * file-related error messages
 */

#define ERRMAS_STAT_FILE(name) \
	_("failed to retrieve information for `%s'"), name

#define ERRMAS_OPEN_FILE(name) \
	_("failed to open `%s'"), name

#define ERRMAS_CREAT_FILE(name) \
	_("failed to create file at `%s'"), name

#define ERRMAS_CREAT_DIR(name) \
	_("failed to make directory `%s'"), name

#define ERRMAS_CREAT_LINK(target, name) \
	_("failed to make symbolic link `%s' for target `%s'"), name, target

#define ERRMAS_ACCESS_FILE(name) \
	_("failed to access file `%s'"), name

#define ERRMAS_READ_FILE(name) \
	_("failed to read file `%s'"), name

#define ERRMAS_WRITE_FILE(name) \
	_("failed to write to file `%s'"), name

#define ERRMAS_READ_LINK(name) \
	_("unable to read link `%s'"), name

#define ERRMAS_COPY_FILE(src, dest) \
	_("unable to copy file from `%s' to `%s'"), src, dest

#define ERRMAS_RENAME_FILE(src, dest) \
	_("failed to rename file `%s' to `%s'"), src, dest

#define ERRMAS_REMOVE_FILE(name) \
	_("failed to remove file `%s'"), name

#define ERRMAS_WAIT_PROC(id) \
	_("failed to wait for process `%d' to terminate"), id

#endif /* TERMAS_H */
