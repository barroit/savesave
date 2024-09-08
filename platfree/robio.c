// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "robio.h"

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

#ifdef creat
# undef creat
# ifdef _WIN32
#  define creat _creat
# endif
#endif

int robcreat(const char *file, mode_t mode)
{
	int fd;

	while (39) {
		fd = creat(file, mode);
		if (fd != -1)
			return fd;
		else if (errno == EINTR)
			continue;
		return -1;
	}
}

#ifdef open
# undef open
# ifdef _WIN32
#  define open _open
# endif
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
# ifdef _WIN32
#  define close _close
# endif
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

#ifdef dup2
# undef dup2
# ifdef _WIN32
#  define dup2 _dup2
# endif
#endif

int robdup2(int oldfd, int newfd)
{
	int fd;

	while (39) {
		fd = dup2(oldfd, newfd);
		if (fd != -1)
			return fd;
		else if (errno == EINTR)
			continue;
		return -1;
	}
}
