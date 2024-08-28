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
	int do_dup;
};

#define STRLIST_INIT { 0 }

#define STRLIST_DUPSTR 1 << 0

void strlist_init(struct strlist *sl, flag_t flags);

/*
 * A destroyed strlist object can be re-initialised using strlist_init();
 */
void strlist_destroy(struct strlist *sl);

void strlist_push(struct strlist *sl, const char *str);

char *strlist_pop(struct strlist *sl);

/*
 * Make strlist more ‘argv-like’
 */
void strlist_terminate(struct strlist *sl);

#ifdef __cplusplus
}
#endif

#endif /* STRLIST_H */
