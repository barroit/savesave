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
	const char *relname;
	const char *basname;

	int fd;
	struct stat *st;
};

typedef int (*fileiter_callback)(struct fileiter_file *src, void *data);

#define FILEITER_CALLBACK(name) \
	int name(struct fileiter_file *src, void *data)

struct fileiter {
	const char *root;

	struct strbuf *sb;
	struct strlist *sl;

	fileiter_callback cb;
	void *data;
};

void fileiter_init(struct fileiter *ctx, const char *head,
		   fileiter_callback cb, void *data);

void fileiter_destroy(struct fileiter *ctx);

int fileiter_exec(struct fileiter *ctx);

#ifdef __cplusplus
}
#endif

#endif /* FILEITER_H */
