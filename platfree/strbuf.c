// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strbuf.h"
#include "alloc.h"
#include "debug.h"
#include "list.h"
#include "mkdir.h"

void strbuf_init(struct strbuf *sb, flag_t flags)
{
	memset(sb, 0, sizeof(*sb));

	if (flags & STRBUF_CONSTANT)
		sb->is_const = 1;
}

struct strbuf strbuf_from2(const char *base, flag_t flags, size_t extalloc)
{
	BUG_ON(flags & STRBUF_CONSTANT);

	struct strbuf sb = STRBUF_INIT;

	sb.baslen = strbuf_concat2(&sb, base, extalloc);
	return sb;
}

void strbuf_destroy(struct strbuf *sb)
{
	free(sb->str);
	sb->str = STRBUF_POISON;
	sb->cap = 0;
}

void strbuf_reset_from(struct strbuf *sb, const char *base)
{
	sb->len = 0;
	sb->baslen = strbuf_concat(sb, base);
}

void strbuf_require_cap(struct strbuf *sb, size_t n)
{
	CAP_ALLOC(&sb->str, n, &sb->cap);
}

/*
 * Grow capacity by n (exclude null terminator) if the available space is less
 * than space required
 */
static void strbuf_growlen(struct strbuf *sb, size_t n)
{
	CAP_ALLOC(&sb->str, sb->len + n + 1, &sb->cap);
}

size_t strbuf_move(struct strbuf *sb, const char *str)
{
	BUG_ON(!sb->is_const);

	size_t len = strlen(str);

	sb->str = (char *)str;
	sb->len = len;
	sb->cap = len;

	return len;
}

size_t strbuf_concat2(struct strbuf *sb, const char *str, size_t extalloc)
{
	size_t nl = strlen(str);

	strbuf_growlen(sb, nl + extalloc);
	memcpy(sb->str + sb->len, str, nl + 1);
	sb->len += nl;

	return nl;
}

size_t strbuf_concatat(struct strbuf *sb, size_t idx, const char *str)
{
	BUG_ON(idx > sb->len);
	if (idx == sb->len)
		return strbuf_concat(sb, str);

	size_t nl = strlen(str);
	size_t alloc = 0;
	size_t overlap = sb->len - (idx + 1);

	if (nl > overlap) {
		alloc = nl - overlap;
		strbuf_growlen(sb, alloc);
	}

	memcpy(&sb->str[idx], str, nl + 1);
	sb->len += alloc;
	return nl;
}

size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...)
{
	va_list ap, cp;
	va_start(ap, fmt);
	va_copy(cp, ap);

	int nr = vsnprintf(NULL, 0, fmt, cp);
	BUG_ON(nr < 0);

	va_end(cp);
	strbuf_growlen(sb, nr);

	nr = vsnprintf(sb->str + sb->len, nr + 1, fmt, ap);
	BUG_ON(nr < 0);

	va_end(ap);
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

size_t strbuf_cntchr(struct strbuf *sb, int c)
{
	BUG_ON(!sb->str);

	size_t cnt = 0;
	const char *p = sb->str;

	while (*p)
		if (*p++ == c)
			cnt++;

	return cnt;
}

void strbuf_normalize_path(struct strbuf *sb)
{
	size_t i;
	for_each_idx(i, sb->len)
		if (sb->str[i] == '\\')
			sb->str[i] = '/';
}

int strbuf_mkfdirp(struct strbuf *sb)
{
	int err;
	char *p = strbuf_strrsep(sb);

	*p = 0;
	err = mkdirp2(sb->str, sb->baslen);
	if (err)
		return err;

	*p = '/';
	return 0;
}

void strbuf_to_dirname(struct strbuf *sb)
{
	char *dirsep = strbuf_strrsep(sb);
	size_t dirlen = dirsep - sb->str;

	strbuf_trunc(sb, sb->len - dirlen);
}

size_t strbuf_concat_path(struct strbuf *sb,
			  const char *prefix, const char *name)
{
	size_t plen = strlen(prefix);
	size_t nlen = strlen(name);
	size_t len = plen + nlen + 1;

	strbuf_growlen(sb, len);
	memcpy(&sb->str[sb->len], prefix, plen);
	sb->len += plen;

	sb->str[sb->len] = '/';
	sb->len += 1;

	memcpy(&sb->str[sb->len], name, nlen);
	sb->len += nlen;

	return len;
}
