// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strbuf.h"
#include "alloc.h"
#include "debug.h"

void strbuf_init(struct strbuf *sb, flag_t flags)
{
	memset(sb, 0, sizeof(*sb));

	if (flags & STRBUF_CONSTANT)
		sb->is_const = 1;
}

void strbuf_destroy(struct strbuf *sb)
{
	free(sb->str);
}

/*
 * Grow strbuf capacity by n (exclude null terminator)
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

size_t strbuf_concat(struct strbuf *sb, const char *str)
{
	return strbuf_concat2(sb, str, 0);
}

size_t strbuf_concat2(struct strbuf *sb, const char *str, size_t extalloc)
{
	size_t nl = strlen(str);

	strbuf_growlen(sb, nl + extalloc);
	memcpy(sb->str + sb->len, str, nl + 1);
	sb->len += nl;

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

void strbuf_truncate(struct strbuf *sb, size_t n)
{
	sb->len -= n;
	sb->str[sb->len] = 0;
}

void strbuf_trim(struct strbuf *sb)
{
	char *h = sb->str;
	while (*h == ' ')
		h++;

	if (*h == 0) {
		sb->str[0] = 0;
		sb->len = 0;
		return;
	}

	char *t = &sb->str[sb->len];
	while (*(t - 1) == ' ')
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

void str_replace(char *s, int c, int v)
{
	while ((s = strchr(s, c)) != NULL)
		*s++ = v;
}
