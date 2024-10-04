// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "alloc.h"
#include "list.h"

void strlist_init(struct strlist *sl, flag_t flags)
{
	memset(sl, 0, sizeof(*sl));

	if (flags & STRLIST_USEREF)
		sl->use_ref = 1;
}

void strlist_destroy(struct strlist *sl)
{
	if (!sl->use_ref) {
		size_t i;
		for_each_idx(i, sl->cap) {
			struct strbuf *sb = &sl->list[i];
			if (sb->cap)
				strbuf_destroy(sb);
		}
	}

	free(sl->list);
	sl->list = STRLIST_POISON;
	sl->cap = 0;
}

void strlist_reset(struct strlist *sl)
{
	BUG_ON(sl->use_ref);
	while (strlist_pop2(sl, 0));
}

static void strlist_grow1(struct strlist *sl)
{
	CAP_ALLOC(&sl->list, sl->nl + 1, &sl->cap);

	size_t i = sl->uninit;
	for_each_idx_from(i, sl->cap)
		sl->list[i].cap = 0; /* cap as a marker */
}

static void strlist_init_strbuf(struct strlist *sl, struct strbuf *sb)
{
	flag_t flag = 0;

	if (sl->use_ref)
		flag |= STRBUF_CONSTANT;

	strbuf_init(sb, flag);
}

size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc)
{
	BUG_ON(sl->use_ref && extalloc);

	if (sl->nl == sl->cap)
		strlist_grow1(sl);

	struct strbuf *sb = &sl->list[sl->nl++];
	if (!sb->cap)
		strlist_init_strbuf(sl, sb);

	/*
	 * keep track uninitialized strbuf position to avoid unexpacted
	 * zore-cap in strlist_grow1()
	 */
	if (sl->uninit <= sl->nl)
		sl->uninit = sl->nl;

	if (sl->use_ref)
		return strbuf_move(sb, str);
	else
		return strbuf_concat2(sb, str, extalloc);
}

void strlist_join_argv(struct strlist *sl, const char **argv)
{
	for_each_str(argv)
		strlist_push(sl, *argv);
}

void __strlist_join_member(struct strlist *sl, void *arr,
			   size_t nmemb, size_t size, size_t offset)
{
	intptr_t addr = (intptr_t)arr;
	intptr_t end = st_add(addr, st_mult(nmemb, size));

	for (; addr != end; addr += nmemb)
		strlist_push(sl, *(const char **)(addr + offset));
}

char *strlist_pop2(struct strlist *sl, int dup)
{
	if (sl->nl == 0)
		return NULL;

	struct strbuf *sb = &sl->list[--sl->nl];
	char *str = sb->str;
	if (dup)
		str = xstrdup(str);

	strbuf_reset_length(sb);
	return str;
}

char **strlist_dump2(struct strlist *sl, int copy)
{
	size_t i;
	size_t nl = sl->nl;
	char **arr = xreallocarray(NULL, sizeof((*sl->list).str), sl->nl + 1);

	for_each_idx(i, nl)
		if (copy)
			arr[i] = xstrdup(sl->list[i].str);
		else
			arr[i] = sl->list[i].str;

	arr[nl] = NULL;
	return arr;
}

void strlist_split_word(struct strlist *sl, const char *str, uint bound)
{
	size_t tot = strlen(str);
	char *ptr = xstrdup(str);
	char *mp = ptr;
	char *tail = &ptr[tot];

	while (ptr < tail) {
		if (ptr + bound >= tail)
			bound = tail - ptr;

		char *p = &ptr[bound];
		uint l = bound;

		if (*p != 0)
			while (!(isascii(*p) && isspace(*p))) {
				p--;
				l--;
			}

		*p = 0;
		l += 1;

		strlist_push(sl, ptr);
		ptr += l;
	}

	free(mp);
}
