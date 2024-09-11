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
	size_t baslen;

	int is_const;
};

#define STRBUF_INIT { 0 }

#define STRBUF_CONSTANT 1 << 0

void strbuf_init(struct strbuf *sb, flag_t flags);

struct strbuf strbuf_from2(const char *base, flag_t flags, size_t extalloc);

static inline struct strbuf strbuf_from(const char *base, flag_t flags)
{
	return strbuf_from2(base, flags, 0);
}

void strbuf_require_cap(struct strbuf *sb, size_t n);

/*
 * A destroyed strbuf object can be re-initialised using strbuf_init();
 */
void strbuf_destroy(struct strbuf *sb);

static inline void strbuf_zerolen(struct strbuf *sb)
{
	sb->len = 0;
}

static inline void strbuf_reset(struct strbuf *sb)
{
	sb->len = sb->baslen;
}

size_t strbuf_move(struct strbuf *sb, const char *str);

size_t strbuf_concat2(struct strbuf *sb, const char *str, size_t extalloc);

static inline size_t strbuf_concat(struct strbuf *sb, const char *str)
{
	return strbuf_concat2(sb, str, 0);
}

size_t strbuf_concatat(struct strbuf *sb, size_t idx, const char *str);

static inline size_t strbuf_concatat_base(struct strbuf *sb,
					     const char *str)
{
	return strbuf_concatat(sb, sb->baslen, str);
}

size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...);

void strbuf_truncate(struct strbuf *sb, size_t n);

void strbuf_trim(struct strbuf *sb);

size_t strbuf_cntchr(struct strbuf *sb, int c);

void strbuf_normalize_path(struct strbuf *sb);

void strbuf_reset_from(struct strbuf *sb, const char *base);

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H */
