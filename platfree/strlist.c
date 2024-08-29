// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "alloc.h"
#include "strbuf.h"
#include "list.h"
#include "debug.h"

void strlist_init(struct strlist *sl, flag_t flags)
{
	memset(sl, 0, sizeof(*sl));

	if (flags & STRLIST_DUPSTR)
		sl->do_dup = 1;
}

void strlist_destroy(struct strlist *sl)
{
	if (sl->do_dup) {
		size_t i;
		for_each_idx(i, sl->cap) {
			struct strbuf *sb = &sl->list[i];
			if (sb->cap)
				strbuf_destroy(sb);
		}
	}

	free(sl->list);
}

static int strlist_need_grow(struct strlist *sl)
{
	return sl->nl == sl->cap;
}

static void strlist_grow1(struct strlist *sl)
{
	CAP_ALLOC(&sl->list, sl->nl + 1, &sl->cap);

	size_t i = sl->uninit;
	for_each_idx_from(i, sl->cap)
		sl->list[i].cap = 0; /* set cap is just enough */
}

static void strlist_init_strbuf(struct strlist *sl, struct strbuf *sb)
{
	flag_t flag = 0;

	if (!sl->do_dup)
		flag |= STRBUF_CONSTANT;

	strbuf_init(sb, flag);
}

size_t strlist_push(struct strlist *sl, const char *str)
{
	return strlist_push2(sl, str, 0);
}

size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc)
{
	BUG_ON(!sl->do_dup && extalloc);

	if (strlist_need_grow(sl))
		strlist_grow1(sl);

	struct strbuf *sb = &sl->list[sl->nl++];
	if (!sb->cap)
		strlist_init_strbuf(sl, sb);

	if (sl->uninit <= sl->nl)
		sl->uninit = sl->nl;

	if (sl->do_dup)
		return strbuf_concat2(sb, str, extalloc);
	else
		return strbuf_move(sb, str);
}

char *strlist_pop2(struct strlist *sl, int dup)
{
	if (sl->nl == 0)
		return NULL;

	struct strbuf *sb = &sl->list[--sl->nl];
	char *str = sb->str;
	if (dup)
		str = xstrdup(str);

	strbuf_zerolen(sb);
	return str;
}

void strlist_terminate(struct strlist *sl)
{
	strlist_grow1(sl);
	memset(&sl->list[sl->nl], 0, sizeof(*sl->list));
}
