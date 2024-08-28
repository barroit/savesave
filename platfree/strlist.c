// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "alloc.h"

void strlist_init(struct strlist *sl, flag_t flags)
{
	memset(sl, 0, sizeof(*sl));

	if (flags & STRLIST_DUPSTR)
		sl->do_dup = 1;
}

void strlist_destroy(struct strlist *sl)
{
	if (sl->do_dup) {
		char *str;
		while ((str = strlist_pop(sl)))
			free(str);
	}

	free(sl->list);
}

static void strlist_grow1(struct strlist *sl)
{
	CAP_ALLOC(&sl->list, sl->size + 1, &sl->cap);
}

void strlist_push(struct strlist *sl, const char *str)
{
	strlist_grow1(sl);

	if (sl->do_dup)
		str = xstrdup(str);

	sl->list[sl->size++] = (char *)str;
}

char *strlist_pop(struct strlist *sl)
{
	if (sl->size == 0)
		return NULL;
	return sl->list[--sl->size];
}

void strlist_terminate(struct strlist *sl)
{
	strlist_grow1(sl);
	sl->list[sl->size] = NULL;
}
