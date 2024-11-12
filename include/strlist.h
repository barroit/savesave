/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef STRLIST_H
#define STRLIST_H

#define STRLIST_INIT { 0 }

#define STRLIST_STORE_REF (1 << 0)	/* store reference instead of
					   allocating new strbuf */
#define STRLIST_CALC_STRLEN (1 << 1)	/* calculate string length, only work
					   when buffer is __cs */

/*
 * Initialize strlist with flags.
 */
void strlist_init(struct strlist *sl, flag_t flags);

/*
 * Destroy a strlist instance.
 * NB: Destroyed strlist object can be re-initialized.
 */
void strlist_destroy(struct strlist *sl);

/*
 * Make strlist factory new without releasing allocated memory.
 */
void strlist_cleanup(struct strlist *sl);

/*
 * Append str to strlist.
 */
uint strlist_push(struct strlist *sl, const char *str);

/*
 * Append argv to strlist (excluding terminated NULL)
 */
void strlist_join_argv(struct strlist *sl, const char **argv);

void __strlist_join_member(struct strlist *sl, void *arr,
			   uint nmemb, uint size, uint offset);

/*
 * Append the specified member (offset) of each element in an array to
 * strlist.
 * NB: The member must be of type const char * or char *.
 */
#define strlist_join_member(sl, arr, size, memb)		\
	__strlist_join_member(sl, arr, sizeof(*arr), size,	\
			      offsetof(typeof(*arr), memb))

/*
 * Similar to strlist_pop(), except it takes an additional argument. If
 * that argument is set to a non-zero value, the return value is a copy
 * of the last element. Otherwise, the return value is simply a pointer
 * pointing to the last element.
 */
char *strlist_pop2(struct strlist *sl, int dup);

/*
 * Remove an element from sl, and return a copy of that element.
 */
static inline char *strlist_pop(struct strlist *sl)
{
	return strlist_pop2(sl, 1);
}

/*
 * Get string at specified position.
 */
static inline char *strlist_at(struct strlist *sl, uint index)
{
	if (!(sl->flags & STRLIST_STORE_REF))
		return sl->__sb[index].str;
	else
		return (char *)sl->__cs[index];
}

/*
 * Same as strlist_dump().
 *
 * I'm sick of this shit, explaining dup or copy again and again. You
 * know what it does! You just make sure the string's lifecycle is not
 * over during use.
 */
char **strlist_dump2(struct strlist *sl, int copy);

/*
 * Dump elements of strlist into array, terminate the array with NULL.
 */
static inline char **strlist_dump(struct strlist *sl)
{
	return strlist_dump2(sl, 1);
}

/*
 * Splits string into strlist, ensuring that words remain intact and
 * are not broken into pieces.
 */
void strlist_split_word(struct strlist *sl, const char *str, uint bound);

#endif /* STRLIST_H */
