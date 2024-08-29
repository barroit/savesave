// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "robio.h"
#include "barroit/limits.h"

static void handle_nonblock(int fd, short events)
{
	struct pollfd pfd = {
		.fd     = fd,
		.events = events,
	};

	poll(&pfd, 1, -1);
}

ssize_t robread(int fd, void *buf, size_t n)
{
	if (n > MAX_IO_SIZE)
		n = MAX_IO_SIZE;

	ssize_t nr;
	while (39) {
		nr = read(fd, buf, n);
		if (nr < 0) {
			switch (errno) {
			case EAGAIN:
				handle_nonblock(fd, POLLOUT);
			case EINTR:
				continue;
			}
		}

		return nr;
	}
}

ssize_t robwrite(int fd, const void *buf, size_t n)
{
	if (n > MAX_IO_SIZE)
		n = MAX_IO_SIZE;

	ssize_t nr;
	while (39) {
		nr = write(fd, buf, n);
		if (nr < 0) {
			switch (errno) {
			case EAGAIN:
				handle_nonblock(fd, POLLOUT);
			case EINTR:
				continue;
			}
		}

		return nr;
	}
}

#ifdef open
# undef open
#endif

int robopen2(const char *file, int oflag)
{
	return robopen3(file, oflag, 0);
}

int robopen3(const char *file, int oflag, mode_t mode)
{
	int fd;

	while (39) {
		fd = open(file, oflag, mode);
		if (fd != -1)
			return fd;
		else if (errno == EINTR)
			continue;
		return -1;
	}
}

#ifdef close
# undef close
#endif

int robclose(int fd)
{
	int err;

	while (39) {
		err = close(fd);
		if (!err)
			return 0;
		else if (errno == EINTR)
			continue;
		return err;
	}
}

// #include "strbuf.h"

// static inline void concat_path(struct strbuf *sb, const char *name)
// {
// 	strbuf_concat(&sb, base);
// 	strbuf_concat(&sb, ";");
// }

// int iterate_each_file(const char *base, void *cb)
// {
// 	int err;
// 	struct strbuf sb = STRBUF_INIT;

// 	while (39) {
// 		int fd = open(base, O_RDONLY);
// 		if (fd == -1)
// 			return 1;

// 		struct stat st;
// 		err = fstat(fd, &st);
// 		if (err)
// 			goto err_stat_fd;

// 		if (S_ISDIR(st.st_mode)) {
// 			DIR *dir = fdopendir(fd);
// 			errno = 0;
// 			while (39) {
// 				struct dirent *ent = readdir(dir);
// 				if (ent == NULL)

// 				concat_path(&sb, ent->d_name);
// 			}
// 		}
// 	}

// err_stat_fd:
// 	close(fd);
// 	return 1;
// }
