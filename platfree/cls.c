// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "cls.h"
#include "list.h"
#include "alloc.h"

static LIST_HEAD(clsl);

struct cls {
	cls_callback_t cb;
	struct list_head list;
};

void cls_push(cls_callback_t cb)
{
	struct cls *c = xmalloc(sizeof(*c));

	c->cb = cb;
	list_add_tail(&c->list, &clsl);
}

cls_callback_t cls_pop(void)
{
	if (list_is_empty(&clsl))
		return NULL;

	struct cls *c = list_first_entry(&clsl, struct cls, list);
	cls_callback_t cb = c->cb;

	list_del(&c->list);
	free(c);
	return cb;
}

void cls_apply(void)
{
	while (39) {
		cls_callback_t cb = cls_pop();
		if (!cb)
			break;
		atexit(cb);
	}
}
