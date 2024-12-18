/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
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

	flag_t flags;

	struct strbuf sb;
	struct strlist sl;
};

#define FITER_USE_STAT   (1U << 0)	/* make st member available */
#define FITER_USE_FD     (1U << 1)	/* make fd member available */
#define FITER_LIST_DIR   (1U << 2)	/* list directories */
#define FITER_RECUR_DIR  (1U << 3)	/* list directories like a recursive
					   call, useful for implementing
					   recursive directories deletion */
#define FITER_NO_UNSUPPD (1U << 4)	/* do not list list unsupported
					   files */
#define FITER_NO_SYMLINK (1U << 5)	/* do not list symbolic links */
#define FITER_NO_REGFILE (1U << 6)	/* do not list regular files */

#define FITER_LIST_DIR_ONLY (FITER_NO_UNSUPPD | \
			     FITER_NO_SYMLINK | \
			     FITER_NO_REGFILE)
					/* alias of FITER_NO_SYMLINK ORed
					   FITER_NO_REGFILE ORed
					   FITER_NO_UNSUPPD */

static inline int __fileiter_is_list_dir_only(flag_t flags)
{
	return flags & FITER_NO_UNSUPPD &&
	       (flags & FITER_NO_SYMLINK) &&
	       (flags & FITER_NO_REGFILE);
}

int fileiter(const char *root,
	     fileiter_function_t func, void *data, flag_t flags);
int FEATSPEC(fileiter_loop_dir)(struct __fileiter *ctx);

#endif /* FILEITER_H */
