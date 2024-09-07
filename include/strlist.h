/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRLIST_H
#define STRLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "strbuf.h"

struct strlist {
	struct strbuf *list;
	size_t uninit;
	size_t nl;
	size_t cap;

	int use_move;
};

#define STRLIST_INIT { 0 }

#define STRLIST_USEMOVE 1 << 0

void strlist_init(struct strlist *sl, flag_t flags);

/*
 * A destroyed strlist object can be re-initialised using strlist_init();
 */
void strlist_destroy(struct strlist *sl);

size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc);

static inline size_t strlist_push(struct strlist *sl, const char *str)
{
	return strlist_push2(sl, str, 0);
}

char *strlist_pop2(struct strlist *sl, int dup);

static inline char *strlist_pop(struct strlist *sl)
{
	return strlist_pop2(sl, 1);
}

char **strlist_dump(struct strlist *sl);

void destroy_dumped_strlist(char **arr);

void strlist_strsplt_every(struct strlist *sl, const char *str, size_t len);

/*
 * same as strlist_strsplt_every but align to previous word boundary
 */
void strlist_strsplt_every2(struct strlist *sl, const char *str, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* STRLIST_H */
