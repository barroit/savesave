/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef FILEITER_H
#define FILEITER_H

#ifdef __cplusplus
extern "C" {
#endif

struct fileiter_file {
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

typedef int (*fileiter_callback_t)(struct fileiter_file *file, void *data);

struct fileiter {
	const char *root;

	struct strbuf *sb;
	struct strlist *sl;

	fileiter_callback_t cb;
	void *data;

	flag_t flag;
};

#define FI_USE_STAT   (1 << 0)
#define FI_USE_FD     (1 << 1)
#define FI_LIST_DIR   (1 << 2)
#define FI_LOOP_UNSUP (1 << 3)

void fileiter_init(struct fileiter *ctx, const char *root,
		   fileiter_callback_t cb, void *data, flag_t flags);

void fileiter_destroy(struct fileiter *ctx);

int PLATSPECOF(fileiter_do_exec)(struct fileiter *ctx);

int fileiter_exec(struct fileiter *ctx);

#ifdef __cplusplus
}
#endif

#endif /* FILEITER_H */
