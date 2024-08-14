/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRBUF_H
#define STRBUF_H

#ifdef __cplusplus
extern "C" {
#endif

struct strbuf {
	char *str;
	size_t len;
	size_t cap;
};

#define STRBUF_INIT { 0 }

void strbuf_grow(struct strbuf *sb, size_t nl);

size_t strbuf_append(struct strbuf *sb, const char *str);

size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...);

void strbuf_truncate(struct strbuf *sb, size_t n);

static inline void strbuf_destory(struct strbuf *sb)
{
	free(sb->str);
}

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H */
