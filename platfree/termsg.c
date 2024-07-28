// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"
#include "barroit/io.h"
#include "list.h"

#define UPDATE_BUF(buf, n, len)		\
	do {				\
		size_t __len = len;	\
		*(n) -= __len;		\
		*(buf) += __len;	\
	} while (0)

static void vreport_prefix(char **buf, size_t *avail, const char *lead)
{
	size_t n = strlen(lead);
	if (n > *avail) {
		fprintf(stderr, "BUG!!! too long a prefix ‘%.10s...’\n", lead);
		abort();
	}

	memcpy(*buf, lead, n);
	UPDATE_BUF(buf, avail, n);
}

static void vreport_message(char **buf, size_t *avail,
			    const char *fmt, va_list ap)
{
	int nr = vsnprintf(*buf, *avail + 1, /* \0 is unnecessary */ fmt, ap);
	if (nr < 0) {
		fprintf(stderr, "fatal: unable to format message ‘%s’\n", fmt);
		exit(128);
	} else if ((unsigned)nr > *avail) {
		nr = *avail;
	}

	UPDATE_BUF(buf, avail, nr);
}

static void vreport_extra(char **buf, size_t *avail, const char *extr)
{
	memcpy(*buf, "; ", 2);
	UPDATE_BUF(buf, avail, 2);

	size_t n = strlen(extr);
	if (n > *avail)
		n = *avail;

	memcpy(*buf, extr, n);
	UPDATE_BUF(buf, avail, n);
}

static void vreportf(FILE *stream, const char *lead,
		    const char *fmt, va_list ap,
		    const char *extr)
{
	char msg[4096];
	size_t avail = sizeof(msg) - 1; /* -1 for \n */
	char *buf = msg;

	vreport_prefix(&buf, &avail, lead);

	vreport_message(&buf, &avail, fmt, ap);

	if (extr && avail > 2)
		vreport_extra(&buf, &avail, extr);

	for (buf -= 1; buf != msg; buf--) {
		if (iscntrl(*buf) && *buf != '\t' && *buf != '\n')
			*buf = '?';
	}

	size_t n = sizeof(msg) - avail;
	msg[n - 1] = '\n';

	fflush(stream);
	int fd = fileno(stream);
	robwrite(fd, msg, n);
}

void warn_routine(const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, "warning: ", fmt, ap, extr);
	va_end(ap);
}

int error_routine(const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, "error: ", fmt, ap, extr);
	va_end(ap);

	return 1;
}

void die_routine(const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, "fatal: ", fmt, ap, extr);

	exit(128);
}

void bug_routine(const char *file, int line, const char *fmt, ...)
{
	va_list ap;
	char lead[256];

	va_start(ap, fmt);
	snprintf(lead, sizeof(lead), "BUG: %s:%d: ", file, line);
	vreportf(stderr, lead, fmt, ap, NULL);

	exit(128);
}

const char *getstrerror(void)
{
	return strerror(errno);
}
