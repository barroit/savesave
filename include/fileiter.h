/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef FILEITER_H
#define FILEITER_H

struct iterfile {
	const char *absname;	/* full name of file */
	const char *dymname;	/* dynamic part (not necessarily a basename) of
				   path */
	const char *basname;	/* basename of path */

	int fd;
	struct stat st;
};

typedef int (*fileiter_function_t)(struct iterfile *file, void *data);

struct __fileiter {
	const char *root;

	fileiter_function_t func;
	void *data;

	flag_t flag;

	struct strbuf sb;
	struct strlist sl;
};

#define FITER_USE_STAT   (1 << 0)	/* make st member available */
#define FITER_USE_FD     (1 << 1)	/* make fd member available */
#define FITER_LIST_DIR   (1 << 2)	/* list directories */
#define FITER_RECUR_DIR  (1 << 3)	/* list directories like a recursive
					   call, useful for implementing
					   recursive directories deletion */
#define FITER_LIST_UNSUP (1 << 4)	/* list unsupported files */
#define FITER_DIR_ONLY   (1 << 5)	/* only list directories */

int fileiter(const char *root,
	     fileiter_function_t func, void *data, flag_t flag);
int PLATSPECOF(fileiter_loop_dir)(struct __fileiter *ctx);

#endif /* FILEITER_H */
