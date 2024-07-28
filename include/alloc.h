// SPDX-License-Identifier: GPL-3.0
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

void memory_limit_check(size_t n);

void *xrealloc(void *p, size_t n);

void *xmalloc(size_t n);

#define REALLOC_ARRAY(p, n) xrealloc(p, st_mult(sizeof(*(p)), n))

void cap_grow(void **buf, size_t req, size_t *cap);

#define CAP_GROW(buf, req, cap) cap_grow((void **)buf, req, cap)

#ifdef __cplusplus
}
#endif

#endif /* ALLOC_H */
