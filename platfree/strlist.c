// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "strbuf.h"
#include "alloc.h"
#include "list.h"

void strlist_init(struct strlist *sl, flag_t flags)
{
	BUG_ON(!(flags & STRLIST_STORE_REF) && (flags & STRLIST_CALC_STRLEN));

	memset(sl, 0, sizeof(*sl));
	sl->flags = flags;
}

void strlist_destroy(struct strlist *sl)
{
	if (sl->flags & STRLIST_STORE_REF)
		goto out;

	struct strbuf *sb = sl->__sb;
	while (sb->cap)
		strbuf_destroy(sb++);

out:
	free(sl->__sb);
	sl->__sb = STRLIST_POISON;
}

void strlist_cleanup(struct strlist *sl)
{
	while (strlist_pop2(sl, 0));
}

static void strlist_nalloc(struct strlist *sl, uint size)
{
	if (!(sl->flags & STRLIST_STORE_REF)) {
		CAP_ALLOC(&sl->__sb, sl->size + size, &sl->cap);

		uint i = sl->alnext;
		for_each_idx_from(i, sl->cap)
			sl->__sb[i].cap = 0; /* cap as a marker */
	} else {
		CAP_ALLOC(&sl->__cs, sl->size + size, &sl->cap);
	}
}

static uint strlist_push_cs(struct strlist *sl, const char *str)
{
	sl->__cs[sl->size++] = str;

	if (likely(!(sl->flags & STRLIST_CALC_STRLEN)))
		return 0;

	return strlen(str);
}

static uint strlist_push_sb(struct strlist *sl, const char *str)
{
	struct strbuf *sb = &sl->__sb[sl->size++];

	if (!sb->cap)
		strbuf_init(sb, 0);

	/*
	 * keep track uninitialized strbuf position to avoid
	 * unexpacted zore-out strbuf cap in strlist_nalloc()
	 */
	if (sl->alnext <= sl->size)
		sl->alnext = sl->size;

	return strbuf_concat(sb, str);
}

uint strlist_push(struct strlist *sl, const char *str)
{
	if (sl->size == sl->cap)
		strlist_nalloc(sl, 1);

	if (!(sl->flags & STRLIST_STORE_REF))
		return strlist_push_sb(sl, str);
	else
		return strlist_push_cs(sl, str);
}

void strlist_join_argv(struct strlist *sl, const char **argv)
{
	for_each_str(argv)
		strlist_push(sl, *argv);
}

void __strlist_join_member(struct strlist *sl, void *arr,
			   uint nmemb, uint size, uint offset)
{
	intptr_t addr = (intptr_t)arr;
	size_t sum = st_umult(nmemb, size);
	intptr_t end = st_uadd(addr, sum);

	for (; addr != end; addr += nmemb)
		strlist_push(sl, *(const char **)(addr + offset));
}

static char *strlist_pop_sb(struct strlist *sl)
{
	struct strbuf *sb = &sl->__sb[--sl->size];
	strbuf_reset_length(sb);

	return sb->str;
}

static char *strlist_pop_cs(struct strlist *sl)
{
	return (char *)sl->__cs[--sl->size];
}

char *strlist_pop2(struct strlist *sl, int dup)
{
	if (!sl->size)
		return NULL;

	char *str;
	if (!(sl->flags & STRLIST_STORE_REF))
		str = strlist_pop_sb(sl);
	else
		str = strlist_pop_cs(sl);

	if (dup)
		str = xstrdup(str);

	return str;
}

char **strlist_dump2(struct strlist *sl, int copy)
{
	uint i;
	char **arr = xreallocarray(NULL, sizeof(*arr), sl->size + 1);

	for_each_idx(i, sl->size) {
		char *str;
		if (!(sl->flags & STRLIST_STORE_REF))
			str = sl->__sb[i].str;
		else
			str = (char *)sl->__cs[i];

		if (copy)
			str = xstrdup(str);
		arr[i] = str;
	}

	arr[sl->size] = NULL;
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
