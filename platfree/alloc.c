// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "barroit/limits.h"
#include "termsg.h"

void memory_limit_check(size_t n)
{
	if (n > MAX_ALLOC_SIZE) {
		die("attempting to allocate %" PRIuMAX " over limit %" PRIuMAX,
		    (uintmax_t)n, (uintmax_t)MAX_ALLOC_SIZE);
	}
}

void *xrealloc(void *p, size_t n)
{
	memory_limit_check(n);

	p = realloc(p, n);
	if (!p)
		die("out of memory (tried to allocate %" PRIuMAX " bytes)", n);
	return p;
}

void *xmalloc(size_t n)
{
	memory_limit_check(n);

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

void cap_grow(void **buf, size_t req, size_t *cap)
{
	if (req > *(cap)) {
		*(cap) = fix_grow(*(cap));
		if (*(cap) < req)
			*(cap) = req;
		*(buf) = REALLOC_ARRAY(*(buf), *(cap));
	}
}
