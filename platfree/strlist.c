// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "alloc.h"
#include "list.h"
#include "debug.h"

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

	if (sl->use_ref)
		flag |= STRBUF_CONSTANT;

	strbuf_init(sb, flag);
}

size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc)
{
	BUG_ON(sl->use_ref && extalloc);

	if (strlist_need_grow(sl))
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

void strlist_destory_dumped(char **arr)
{
	char **p = arr;
	while (*p != NULL)
		free(*p++);
	free(arr);
}

void strlist_strsplt_every(struct strlist *sl, const char *str, size_t len)
{
	size_t tot = strlen(str);
	char *ptr = xstrdup(str);
	char *mp = ptr;
	char *tail = &ptr[tot];

	while (ptr < tail) {
		if (ptr + len >= tail)
			len = tail - ptr;

		char tmp = ptr[len];
		ptr[len] = 0;

		strlist_push(sl, ptr);

		ptr += len;
		*ptr = tmp;
	}

	free(mp);
}

void strlist_strsplt_every2(struct strlist *sl, const char *str, size_t len)
{
	size_t tot = strlen(str);
	char *ptr = xstrdup(str);
	char *mp = ptr;
	char *tail = &ptr[tot];

	while (ptr < tail) {
		if (ptr + len >= tail)
			len = tail - ptr;

		char *p = &ptr[len];
		size_t l = len;

		while (*p != 0 && !(isascii(*p) && isspace(*p))) {
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
