// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "cls.h"
#include "list.h"
#include "alloc.h"
#include "constructor.h"

static LIST_HEAD(clsl);

struct cls {
	cls_callback_t cb;
	struct list_head list;
};

void cls_push(cls_callback_t cb)
{
	struct cls *c = xmalloc(sizeof(*c));

	c->cb = cb;
	list_add(&c->list, &clsl);
}

cls_callback_t cls_pop(void)
{
	struct cls *c = list_first_entry(&clsl, struct cls, list);
	cls_callback_t cb = c->cb;

	list_del(&c->list);
	free(c);
	return cb;
}

static void cls_apply(void)
{
	while (39) {
		if (list_is_empty(&clsl))
			break;

		cls_callback_t cb = cls_pop();
		cb();
	}
}

CONSTRUCTOR(setup_atexit)
{
	atexit(cls_apply);
}
