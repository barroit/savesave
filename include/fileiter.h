/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef FILEITER_H
#define FILEITER_H

struct iterfile {
	const char *absname;
	const char *dymname; /* dynamic part of absname */
	const char *basname;

	/*
	 * not available when
	 *  - absname points to symlink on windows
	 */
	struct stat *st;

	int is_lnk;
	int is_dir;

	/*
	 * not available when
	 *  - on windows, due to the significant performance impact
	 *  - absname points to symlink
	 */
	int fd;
};

typedef int (*fileiter_function_t)(struct iterfile *file, void *data);

struct fileiter {
	const char *root;

	struct strbuf *sb;
	struct strlist *sl;

	fileiter_function_t cb;
	void *data;

	flag_t flag;
};

#define FITER_USE_STAT   (1 << 0)	/* make st member available */
#define FITER_USE_FD     (1 << 1)	/* make fd member available */
#define FITER_LIST_DIR   (1 << 2)	/* list directories */
#define FITER_RECUR_DIR  (1 << 3)	/* list directories like a recursive
					   call, useful for implementing
					   recursive directories deletion */
#define FITER_LIST_UNSUP (1 << 4)	/* list unsupported files */

/**
 * fileiter_init - Initialize file iterator
 */
void fileiter_init(struct fileiter *ctx,
		   fileiter_function_t cb, void *data, flag_t flags);

int fileiter_exec(struct fileiter *ctx, const char *root);
int PLATSPECOF(fileiter_loop_dir)(struct fileiter *ctx);

void fileiter_destroy(struct fileiter *ctx);

#endif /* FILEITER_H */
