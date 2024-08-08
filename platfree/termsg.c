// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"
#include "barroit/io.h"
#include "list.h"
#include "alloc.h"

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

struct cntrl_char {
	unsigned *pos;
	size_t nr;
	size_t cap;
};

static void record_cntrl_char(char *str, size_t nr, struct cntrl_char *cntrl)
{
	size_t i;
	for_each_idx(i, nr) {
		if (iscntrl(str[i]) && str[i] != '\t' && str[i] != '\n') {
			CAP_ALLOC(&cntrl->pos, 1, &cntrl->cap);
			cntrl->pos[cntrl->nr++] = i;
		}
	}
}

static void format_cntrl_char_simple(char *str, unsigned *cntrl, size_t nr)
{
	size_t i;
	for_each_idx(i, nr)
		str[cntrl[i]] = '?';
}

/*
 * a flowchart of this algorithm
 *
 * ? is illegal control char
 * \\\ is illegal control char replacement
 * length of replacement is 3
 *
 * def	- "fun?ct?ion      "
 * 1.1	- "fun?ct?ion ion  "
 * 1.2	- "fun?ct?i\\\ion  "
 * 2.1	- "fun?ctct\\\ion  "
 * 2.2	- "fun\\\ct\\\ion  "
 */
static void format_cntrl_char_advanced(char *str, size_t lasidx,
				       size_t off, size_t replen,
				       unsigned *cntrl, size_t nr)
{
	size_t baslen;
	char *base;
	size_t i;

	for_each_idx_back(i, nr - 1) {
		baslen = lasidx - cntrl[i];
		base = str + cntrl[i] + 1;

		if (likely(baslen))
			memmove(base + off, base, baslen);

		base += off;
		memcpy(base - replen, CONFIG_CNTRL_CHAR_REPLACEMENT, replen);
		off -= replen - 1;
		lasidx = cntrl[i] - 1; /* cntrl[i] == 0 is fine */
	}
}

void vreport_format_cntrl_char(char *str, size_t len, size_t *avail)
{
	struct cntrl_char cntrl = { 0 };

	CAP_ALLOC(&cntrl.pos, 4, &cntrl.cap);
	record_cntrl_char(str, len, &cntrl);

	if (!cntrl.nr)
		goto cleanup;

	size_t replen = strlen(CONFIG_CNTRL_CHAR_REPLACEMENT);
	/*
	 * original cntrl character already takes 1 place, so the size we
	 * actually need is len - 1
	 */
	size_t off = cntrl.nr * (replen - 1);
	size_t lasidx = len - 1;
	/*
	 * for consistency, if the offset is beyond to avail, we simply replace
	 * all unexpected control characters with ‘?’
	 */
	if (unlikely(off > *avail)) {
		format_cntrl_char_simple(str, cntrl.pos, cntrl.nr);
	} else {
		format_cntrl_char_advanced(str, lasidx, off, replen,
					   cntrl.pos, cntrl.nr);
	}
	*avail -= off;

cleanup:
	free(cntrl.pos);
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

	vreport_format_cntrl_char(msg, buf - msg, &avail);

	size_t len = sizeof(msg) - avail;
	msg[len - 1] = '\n';

	fflush(stream);
	int fd = fileno(stream);
	robwrite(fd, msg, len);
}

void __warn_routine(const char *pref, const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, pref, fmt, ap, extr);
	va_end(ap);
}

int __error_routine(const char *pref, const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, pref, fmt, ap, extr);
	va_end(ap);

	return 1;
}

void __die_routine(const char *pref, const char *extr, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vreportf(stderr, pref, fmt, ap, extr);

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

int redirect_stdio(const char *name)
{
	int err;

	err = mk_file_dir(name);
	if (err)
		return error_errno("failed to mkdir for log file ‘%s’", name);

	int fd = open(name, O_WRONLY | O_CREAT, 0664);
	if (fd == -1) {
		error_errno("failed to open ‘%s’", name);
		goto err_open_file;
	}

	if (dup2(fd, fileno(stdout)) == -1) {
		error_errno("cannot redirect stdout");
		goto close_file;
	}

	if (dup2(fd, fileno(stderr)) == -1) {
		error_errno("cannot redirect stderr");
		goto close_file;
	}

	return 0;

close_file:
	close(fd);
err_open_file:
	return 1;
}
