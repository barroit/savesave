// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "termas.h"

void alloc_limit_check(size_t n)
{
	if (n > MAX_ALLOC_SIZE) {
		die(_("attempting to allocate "
		    "%" PRIuMAX " over limit %" PRIuMAX ""),
		    (uintmax_t)n, (uintmax_t)MAX_ALLOC_SIZE);
	}
}

void alloced_or_die(void *ptr, size_t size)
{
	if (likely(ptr))
		return;
	die(_("out of memory (tried to allocate %" PRIuMAX " bytes)"), size);
}

void *xrealloc(void *ptr, size_t size)
{
	alloc_limit_check(size);

	ptr = realloc(ptr, size);
	alloced_or_die(ptr, size);

	return ptr;
}

void *xmalloc(size_t size)
{
	alloc_limit_check(size);

	void *ptr = malloc(size);
	alloced_or_die(ptr, size);

	return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
	size_t tot = st_umult(nmemb, size);
	alloc_limit_check(tot);

	void *ptr = calloc(nmemb, size);
	alloced_or_die(ptr, tot);

	return ptr;
}

void *xreallocarray(void *ptr, size_t nmemb, size_t size)
{
	size_t tot = st_umult(nmemb, size);
	alloc_limit_check(tot);

	ptr = xrealloc(ptr, tot);
	alloced_or_die(ptr, tot);

	return ptr;
}

char *xstrdup(const char *src)
{
	char *str = strdup(src);
	if (!str) /* ENOMEM */
		die_errno(_("failed to duplicate string"));
	return str;
}
