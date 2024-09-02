// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "keepref.h"
#include "list.h"
#include "alloc.h"

struct leak_ref {
	struct list_head node;
	char carrier[];
};

static LIST_HEAD(refs);

void __keepref(void *ptr, size_t size)
{
	size_t n = st_add(sizeof(struct leak_ref), size);
	alloc_limit_check(n);

	struct leak_ref *ref = xmalloc(n);
	alloced_or_die(ref, n);

	list_init(&ref->node);
	list_add(&ref->node, &refs);

	memcpy(ref->carrier, ptr, size);
}
