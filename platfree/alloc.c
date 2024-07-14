// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "barroit/limits.h"
#include <stdint.h>
#include "termsg.h"
#include <inttypes.h>
#include <stdlib.h>

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

void cap_grow(void **buf, size_t req, size_t *cap)
{
	if (req > *(cap)) {
		*(cap) = fix_grow(*(cap));
		if (*(cap) < req)
			*(cap) = req;
		*(buf) = REALLOC_ARRAY(*(buf), *(cap));
	}
}
