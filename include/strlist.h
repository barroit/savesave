/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRLIST_H
#define STRLIST_H

#include "strbuf.h"

struct strlist {
	struct strbuf *list;
	size_t uninit;
	size_t nl;
	size_t cap;

	int use_ref;
};

#define STRLIST_INIT { 0 }

#define STRLIST_USEREF 1 << 0

void strlist_init(struct strlist *sl, flag_t flags);

/*
 * A destroyed strlist object can be re-initialised using strlist_init();
 */
void strlist_destroy(struct strlist *sl);

/*
 * strlist_reset - make strlist 'factory new' without releasing
 *		   allocated memory
 */
void strlist_reset(struct strlist *sl);

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

char **strlist_dump2(struct strlist *sl, int copy);

static inline char **strlist_dump(struct strlist *sl)
{
	return strlist_dump2(sl, 1);
}

/*
 * strlist_split_word - split string, make sure the words don't get broken
 *			into pieces
 */
void strlist_split_word(struct strlist *sl, const char *str, uint bound);

#endif /* STRLIST_H */
