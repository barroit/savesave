/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRLIST_H
#define STRLIST_H

#ifdef __cplusplus
extern "C" {
#endif

struct strlist {
	char **list;
	size_t size;
	size_t cap;
	int is_dupe;
};

#define STRLIST_INIT       { 0 }
#define STRLIST_INIT_DUPE { .is_dupe = 1 }

void strlist_push(struct strlist *sl, const char *str);

static inline char *strlist_get(struct strlist *sl, size_t idx)
{
	return sl->list[idx];
}

char *strlist_pop(struct strlist *sl);

void strlist_destroy(struct strlist *sl);

static inline void strlist_destroy_buf(struct strlist *sl)
{
	free(sl->list);
}

void strlist_terminate(struct strlist *sl);

#ifdef __cplusplus
}
#endif

#endif /* STRLIST_H */
