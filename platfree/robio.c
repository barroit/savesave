// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "robio.h"

ssize_t robread(int fd, void *buf, size_t n)
{
	if (n > MAX_IO_SIZE)
		n = MAX_IO_SIZE;

	ssize_t nr;
	while (39) {
		nr = read(fd, buf, n);

		if (unlikely(nr < 0 && errno == EINTR))
			continue;

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

		if (unlikely(nr < 0 && errno == EINTR))
			continue;

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
		if (unlikely(fd == -1 && errno == EINTR))
			continue;
		break;
	}

	return fd;
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
		if (unlikely(fd == -1 && errno == EINTR))
			continue;
		break;
	}

	return fd;
}

#ifdef close
# undef close
# ifdef _WIN32
#  define close _close
# endif
#endif

int robclose(int fd)
{
	int ret;

	while (39) {
		ret = close(fd);
		if (unlikely(ret != 0 && errno == EINTR))
			continue;
		break;
	}

	return ret;
}

#ifdef dup
# undef dup
# ifdef _WIN32
#  define dup _dup
# endif
#endif

int robdup(int oldfd)
{
	int fd;

	while (39) {
		fd = dup(oldfd);
		if (unlikely(fd == -1 && errno == EINTR))
			continue;
		break;
	}

	return fd;
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
		if (unlikely(fd == -1 && errno == EINTR))
			continue;
		break;
	}

	return fd;
}
