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

#include "barroit/types.h"

struct strbuf {
	char *str;
	size_t len;
	size_t cap;
};

#define STRBUF_INIT { 0 }

void strbuf_init(struct strbuf *sb, flag_t flags);

/*
 * A destroyed strbuf object can be re-initialised using strbuf_init();
 */
void strbuf_destroy(struct strbuf *sb);

size_t strbuf_concat(struct strbuf *sb, const char *str);

size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...);

void strbuf_truncate(struct strbuf *sb, size_t n);

void strbuf_trim(struct strbuf *sb);

size_t strbuf_cntchr(struct strbuf *sb, int c);

void str_replace(char *s, int c, int v);

#ifdef __cplusplus
}
#endif

#endif /* STRBUF_H */
