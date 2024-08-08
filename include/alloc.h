/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calc.h"

void alloc_limit_check(size_t n);

void *xrealloc(void *p, size_t n);

void *xmalloc(size_t n);

char *xstrdup(const char *src);

#define REALLOC_ARRAY(p, n) xrealloc(p, st_mult(sizeof(*(p)), n))

void cap_alloc(void **buf, size_t nl, size_t *cap);

/**
 * CAP_ALLOC - increase buf size
 * @buf: pointer points to real buf
 * @nl:  minimum total size that this buffer should have (i.e., len + nl + 1)
 * @cap: pointer points to buf capacity
 */
#define CAP_ALLOC(buf, nl, cap) cap_alloc((void **)buf, nl, cap)

#ifdef __cplusplus
}
#endif

#endif /* ALLOC_H */
