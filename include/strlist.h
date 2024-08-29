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

struct strbuf;

struct strlist {
	struct strbuf *list;
	size_t uninit;
	size_t nl;
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

size_t strlist_push(struct strlist *sl, const char *str);

size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc);

/*
 * return value shall be freed by caller
 */
#define strlist_pop(sl) strlist_pop2(sl, 1)

/*
 * if dup is 0, the return value is valid until the next call to strlist_push()
 */
char *strlist_pop2(struct strlist *sl, int dup);

/*
 * Make strlist more ‘argv-like’
 */
void strlist_terminate(struct strlist *sl);

#ifdef __cplusplus
}
#endif

#endif /* STRLIST_H */
