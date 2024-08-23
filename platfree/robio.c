// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "barroit/io.h"
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
