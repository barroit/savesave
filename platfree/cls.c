// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
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

void cls_add(cls_callback_t cb)
{
	struct cls *c = xmalloc(sizeof(*c));

	c->cb = cb;
	list_add_tail(&c->list, &clsl);
}

cls_callback_t cls_rm(void)
{
	if (list_is_empty(&clsl))
		return NULL;

	struct cls *c = list_first_entry(&clsl, struct cls, list);
	cls_callback_t cb = c->cb;

	list_del(&c->list);
	free(c);
	return cb;
}

static void cls_apply(void)
{
	cls_callback_t cb;
	while ((cb = cls_rm()) != NULL)
		cb();
}

CONSTRUCTOR(setup_atexit)
{
	atexit(cls_apply);
}
