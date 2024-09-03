/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ROBIO_H
#define ROBIO_H

#ifdef __cplusplus
extern "C" {
#endif

ssize_t robread(int fd, void *buf, size_t n);

ssize_t robwrite(int fd, const void *buf, size_t n);

int robopen2(const char *file, int oflag);
int robopen3(const char *file, int oflag, mode_t mode);
#define robopen(...) FLEXCALL_FUNCTION3(robopen, __VA_ARGS__)

int robclose(int fd);

char *readfile(const char *name);

int is_abs_path(const char *path);

static inline int is_dir_indicator(const char *name)
{
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

const char *get_home_dir(void);

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

#endif /* ROBIO_H */
