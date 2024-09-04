/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calc.h"
#include "poison.h"

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_HEAD(name) \
	struct list_head name = { &(name), &(name) }

static inline void list_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *list,
			      struct list_head *prev,
			      struct list_head *next)
{
	prev->next = list;
	list->prev = prev;
	list->next = next;
	next->prev = list;
}

static inline void list_add(struct list_head *list, struct list_head *head)
{
	__list_add(list, head, head->next);
}

static inline void list_add_tail(struct list_head *list,
				 struct list_head *head)
{
	__list_add(list, head->prev, head);
}

#define for_each_idx(i, n) for (i = 0; i < n; i++)

/*
 * handle index up to max_uint_val_of_type(i) - 1
 */
#define for_each_idx_back(i, n) for (i = n; i != max_uint_val_of_type(i); i--)

#define for_each_idx_from(i, n) for (; i < n; i++)

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
