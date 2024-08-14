// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strbuf.h"
#include "alloc.h"
#include "termsg.h"

void strbuf_grow(struct strbuf *sb, size_t nl)
{
	BUG_ON(nl == 0);

	CAP_ALLOC(&sb->str, sb->len + nl, &sb->cap);
}

size_t strbuf_append(struct strbuf *sb, const char *str)
{
	size_t nl = strlen(str);

	strbuf_grow(sb, nl + 1);

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
	if (unlikely(nr < 0))
		goto err_printf;

	va_end(cp);
	strbuf_grow(sb, nr + 1);

	nr = vsnprintf(sb->str + sb->len, nr + 1, fmt, ap);
	if (unlikely(nr < 0))
		goto err_printf;

	va_end(ap);
	sb->len += nr;
	return nr;

err_printf:
		bug("vsnprintf() is broken");
}

void strbuf_truncate(struct strbuf *sb, size_t n)
{
	BUG_ON(n > sb->len);

	sb->len -= n;
	sb->str[sb->len] = 0;
}
