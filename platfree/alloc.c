// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "barroit/limits.h"
#include "termsg.h"

void alloc_limit_check(size_t n)
{
	if (n > MAX_ALLOC_SIZE) {
		die("attempting to allocate %" PRIuMAX " over limit %" PRIuMAX,
		    (uintmax_t)n, (uintmax_t)MAX_ALLOC_SIZE);
	}
}

void *xrealloc(void *p, size_t n)
{
	alloc_limit_check(n);

	p = realloc(p, n);
	if (!p)
		die("out of memory (tried to allocate %" PRIuMAX " bytes)", n);
	return p;
}

void *xmalloc(size_t n)
{
	alloc_limit_check(n);

	void *p = malloc(n);
	if (!p)
		die("out of memory (tried to allocate %" PRIuMAX " bytes)", n);
	return p;
}

char *xstrdup(const char *src)
{
	char *str = strdup(src);
	if (!str)
		die_errno("failed to duplicate string");
	return str;
}
