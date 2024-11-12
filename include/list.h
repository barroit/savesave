/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef LIST_H
#define LIST_H

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void list_head_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev, struct list_head *next)
{
	prev->next = new;
	new->prev = prev;
	new->next = next;
	next->prev = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *list)
{
	__list_del(list->prev, list->next);
	list->next = LIST_POISON1;
	list->prev = LIST_POISON2;
}

static inline int list_is_head(struct list_head *list, struct list_head *head)
{
	return list == head;
}

static inline int list_is_empty(struct list_head *head)
{
	return head->next == head;
}

#define list_entry(ent, type, memb) \
	containerof(ent, type, memb)

#define list_first_entry(head, type, memb) \
	list_entry((head)->next, type, memb)

#define list_next_entry(ent, memb) \
	list_entry(ent->memb.next, typeof(*ent), memb)

#define list_last_entry(head, type, memb) \
	list_entry((head)->prev, type, memb)

#define list_entry_is_head(pos, head, memb) \
	list_is_head(&pos->memb, head)

#define list_for_each_entry(pos, head, memb)			\
	for (pos = list_first_entry(head, typeof(*pos), memb);	\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = list_next_entry(pos, memb))

#define list_for_each_entry_safe(pos, next, head, memb)		\
	for (pos = list_first_entry(head, typeof(*pos), memb),	\
		next = list_next_entry(pos, memb);		\
	     !list_entry_is_head(pos, head, memb); 		\
	     pos = next, next = list_next_entry(next, memb))

#endif /* LIST_H */
