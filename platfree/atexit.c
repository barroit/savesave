// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma GCC diagnostic ignored "-Wparentheses"

#include "atexit.h"
#include "list.h"
#include "alloc.h"

static LIST_HEAD(aearr);

struct atexit {
	atexit_function_t func;
	struct list_head list;
};

void atexit_enque(atexit_function_t func)
{
	struct atexit *ae = xmalloc(sizeof(*ae));

	ae->func = func;
	list_add_tail(&ae->list, &aearr);
}

atexit_function_t atexit_deque(void)
{
	if (list_is_empty(&aearr))
		return NULL;

	struct atexit *ent = list_last_entry(&aearr, struct atexit, list);
	atexit_function_t func = ent->func;

	list_del(&ent->list);
	free(ent);

	return func;
}

void atexit_apply(void)
{
	atexit_function_t func;

	while (func = atexit_deque())
		atexit(func);
}
