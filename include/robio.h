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

const char *get_home_dir(void);

/*
 * file_iter - iterate files from given path
 *
 * head - first file to iterate
 * cb	- callback of file iterator, if the processing file is a symlink, fd is
 *	  set to -1 and st is set to NULL
 * data	- data passed to callback function
 */
struct file_iter {
	const char *head;

	struct strbuf *sb;
	struct strlist *sl;

	int (*cb)(const char *name, int fd, struct stat *st, void *data);
	void *data;
};

typedef typeof_member(struct file_iter, cb) file_iterator_cb_t;

void file_iter_init(struct file_iter *ctx, const char *head,
			file_iterator_cb_t cb, void *data);

void file_iter_destroy(struct file_iter *ctx);

int file_iter_exec(struct file_iter *ctx);

#ifdef __cplusplus
}
#endif

#endif /* ROBIO_H */
