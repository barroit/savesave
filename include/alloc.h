/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ALLOC_H
#define ALLOC_H

void alloc_limit_check(size_t n);

void alloced_or_die(void *ptr, size_t size);

void *xrealloc(void *p, size_t size);

void *xmalloc(size_t size);

void *xcalloc(size_t nmemb, size_t size);

void *xreallocarray(void *ptr, size_t nmemb, size_t size);

char *xstrdup(const char *src);

#define REALLOC_ARRAY(p, n) xrealloc(p, st_umult(sizeof(*(p)), n))

/**
 * CAP_ALLOC - increase buf size
 * @buf: pointer points to real buf
 * @nl:  minimum total size that this buffer should have
 * 	 (i.e., strlen + nl + 1)
 * @cap: pointer points to buf capacity
 */
#define CAP_ALLOC(p, nl, cap)					\
do {								\
	if ((nl) > *(cap)) {					\
		*(cap) = fix_grow(*cap);			\
		if (*(cap) < (nl))				\
			*(cap) = nl;				\
		*(p) = REALLOC_ARRAY(*(p), *(cap));		\
	}							\
} while (0)
#endif /* ALLOC_H */
