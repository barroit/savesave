// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * NB:	The behavior is undefined if NULL or an empty string is passed
 *	to strbuf_*concat() or strbuf_*printf().
 */

#include "strbuf.h"
#include "alloc.h"
#include "iter.h"
#include "mkdir.h"

void strbuf_init2(struct strbuf *sb, const char *base, flag_t flags)
{
	BUG_ON(flags);

	memset(sb, 0, sizeof(*sb));
	if (!base)
		return;

	sb->base = strbuf_concat(sb, base);
}

void strbuf_destroy(struct strbuf *sb)
{
	BUG_ON(!sb->cap);

	free(sb->str);
	sb->str = STRBUF_POISON;
}

void strbuf_reset_from(struct strbuf *sb, const char *base)
{
	sb->len = 0;
	sb->base = strbuf_concat(sb, base);
}

/*
 * Grow capacity by n (exclude null terminator) if the available space
 * is less than space required.
 */
static void strbuf_nalloc(struct strbuf *sb, size_t n)
{
	CAP_ALLOC(&sb->str, sb->len + n + 1, &sb->cap);
}

uint strbuf_oconcat(struct strbuf *sb, uint offset, const char *str)
{
	uint len = strlen(str);
	uint overlap = sb->len - offset;

	if (len > overlap)
		strbuf_nalloc(sb, len - overlap);

	memcpy(&sb->str[offset], str, len + 1);
	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	return len;
}

uint strbuf_oconcat_char(struct strbuf *sb, uint offset, int c)
{
	uint overlap = sb->len - offset;
	if (!overlap)
		strbuf_nalloc(sb, 1);

	sb->str[offset] = c;
	if (!overlap)
		sb->len += 1;
	else if (overlap > 1)
		sb->len -= overlap - 1;

	sb->str[sb->len] = 0;
	return 1;
}

uint strbuf_oprintf(struct strbuf *sb,
		    uint offset, const char *format, ...)
{
	va_list ap[2];
	va_start(ap[0], format);
	va_copy(ap[1], ap[0]);

	strbuf_nalloc(sb, 32);

	int nr;
	uint avail;
	uint i = 0;

retry:
	avail = strbuf_avail(sb);
	nr = vsnprintf(&sb->str[offset], avail + 1, format, ap[i]);

	BUG_ON(nr < 0);
	if (nr > avail) {
		strbuf_nalloc(sb, nr);
		i += 1;
		goto retry;
	}

	va_end(ap[0]);
	va_end(ap[1]);

	sb->len += nr;
	return nr;
}

void strbuf_trim(struct strbuf *sb)
{
	char *h = sb->str;
	while (isspace(*h))
		h++;

	if (*h == 0) {
		sb->str[0] = 0;
		sb->len = 0;
		return;
	}

	char *t = &sb->str[sb->len];
	while (isspace(*(t - 1)))
		t--;

	if (h == sb->str && t == &sb->str[sb->len])
		return;

	sb->len = t - h;
	memmove(sb->str, h, sb->len);
	sb->str[sb->len] = 0;
}

uint strbuf_count_char(struct strbuf *sb, int c)
{
	uint n = 0;
	const char *str = sb->str;

	while (*str)
		if (*str++ == c)
			n++;
	return n;
}

void strbuf_normalize_path(struct strbuf *sb)
{
	size_t i;
	for_each_idx(i, sb->len)
		if (sb->str[i] == '\\')
			sb->str[i] = '/';

	char *sep = strbuf_strrsep(sb);
	if (sep && sep[1] == 0) {
		int need_sync = sb->base == sb->len;

		strbuf_trunc(sb, 1);
		if (need_sync)
			sb->base = sb->len;
	}
}

void strbuf_dirname(struct strbuf *sb)
{
	char *dirsep = strbuf_strrsep(sb);
	size_t dirlen = dirsep - sb->str;

	strbuf_trunc(sb, sb->len - dirlen);
}

uint strbuf_concat_basename(struct strbuf *sb, const char *name)
{
	uint nsz = strlen(name);
	uint n = nsz + 1;

	strbuf_nalloc(sb, n);

	sb->str[sb->len] = '/';
	sb->len += 1;

	memcpy(&sb->str[sb->len], name, n);
	sb->len += nsz;
	return n;
}

uint strbuf_concat_pathname(struct strbuf *sb,
			    const char *prefix, const char *name)
{
	uint dsz = strlen(prefix);
	uint nsz = strlen(name);
	uint n = dsz + nsz + 1;

	strbuf_nalloc(sb, n);
	memcpy(&sb->str[sb->len], prefix, dsz);
	sb->len += dsz;

	sb->str[sb->len] = '/';
	sb->len += 1;

	memcpy(&sb->str[sb->len], name, nsz + 1);
	sb->len += nsz;
	return n;
}
