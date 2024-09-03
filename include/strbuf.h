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

	int is_const;
};

#define STRBUF_INIT { 0 }

#define STRBUF_CONSTANT 1 << 0

void strbuf_init(struct strbuf *sb, flag_t flags);

/*
 * A destroyed strbuf object can be re-initialised using strbuf_init();
 */
void strbuf_destroy(struct strbuf *sb);

static inline void strbuf_zerolen(struct strbuf *sb)
{
	sb->len = 0;
}

size_t strbuf_move(struct strbuf *sb, const char *str);

size_t strbuf_concat(struct strbuf *sb, const char *str);

size_t strbuf_concat2(struct strbuf *sb, const char *str, size_t extalloc);

size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...);

void strbuf_truncate(struct strbuf *sb, size_t n);

void strbuf_trim(struct strbuf *sb);

size_t strbuf_cntchr(struct strbuf *sb, int c);

void strrepl(char *s, int c, int v);

const char *straftr(const char *str, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H */
