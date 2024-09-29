// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "noleak.h"
#include "list.h"
#include "alloc.h"

struct leakref {
	struct list_head list;
	char carrier[];
};

static LIST_HEAD(refs);

void __noleak(void *ptr, size_t size)
{
	size_t n = st_add(sizeof(struct leakref), size);
	alloc_limit_check(n);

	struct leakref *ref = xmalloc(n);

	list_add(&ref->list, &refs);
	memcpy(ref->carrier, ptr, size);
}
