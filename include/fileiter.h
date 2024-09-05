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

struct file_iter_src {
	const char *absname;
	const char *relname;

	int fd;
	struct stat *st;
};

typedef int (*file_iterator_cb_t)(struct file_iter_src *src, void *data);

#define FILE_ITER_CALLBACK(name) \
	int name(struct file_iter_src *src, void *data)

struct file_iter {
	const char *root;

	struct strbuf *sb;
	struct strlist *sl;

	file_iterator_cb_t cb;
	void *data;
};

void file_iter_init(struct file_iter *ctx, const char *head,
		    file_iterator_cb_t cb, void *data);

void file_iter_destroy(struct file_iter *ctx);

int file_iter_exec(struct file_iter *ctx);

#ifdef __cplusplus
}
#endif

#endif /* FILEITER_H */
