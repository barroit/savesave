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

#define CAP_GROW(p, req, cap)					\
	do {							\
		if (req > *(cap)) {				\
			*(cap) = fix_grow(*(cap));		\
			if (*(cap) < req)			\
				*(cap) = req;			\
			*(p) = REALLOC_ARRAY(*(p), *(cap));	\
		}						\
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* ALLOC_H */
