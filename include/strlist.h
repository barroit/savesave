/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRLIST_H
#define STRLIST_H

#include "strbuf.h"

#define STRLIST_INIT { 0 }

#define STRLIST_USEREF 1 << 0

/**
 * strlist_init - Initialize strlist with flags
 */
void strlist_init(struct strlist *sl, flag_t flags);

/**
 * strlist_destroy - Destroy strlist
 *
 * note: A destroyed strlist object can be re-initialized using strlist_init()
 */
void strlist_destroy(struct strlist *sl);

/**
 * strlist_reset - Make strlist factory new without releasing allocated memory
 */
void strlist_reset(struct strlist *sl);

/**
 * strlist_push2 - Append str to sl, with pre-allocate area
 * 
 * note: The pre-allocate area is applied to the element of internal
 *	 array (strbuf) not the internal array itself
 */
size_t strlist_push2(struct strlist *sl, const char *str, size_t extalloc);

/**
 * strlist_push - Append str to sl
 */
static inline size_t strlist_push(struct strlist *sl, const char *str)
{
	return strlist_push2(sl, str, 0);
}

/**
 * strlist_join_argv - Append entire argv to sl (not including terminated NULL)
 */
void strlist_join_argv(struct strlist *sl, const char **argv);

void __strlist_join_member(struct strlist *sl, void *arr,
			   size_t nmemb, size_t size, size_t offset);

/**
 * strlist_join_member - Append the specified member of each element in an
 *			 array to sl
 *
 * note: Member must be of type const char * or char *
 */
#define strlist_join_member(sl, arr, nl, memb) \
	__strlist_join_member(sl, arr, sizeof(*arr), nl, \
			      offsetof(typeof(*savarr), memb))

/**
 * strlist_pop2 - Remove an element from sl, and return the value of element
 *
 * dup: If this value is zero, the return value is simply a pointer points to
 *	the last element, the value is guarantee to be available until the next
 *	calls to strlist_push() or strlist_push2(). Otherwise, the return value
 *	is a copy of that element
 *
 * note: strlist_pop2 does not release the memory allocated for element
 */
char *strlist_pop2(struct strlist *sl, int dup);


/**
 * strlist_pop - remove an element from sl, and return a copy of that element
 *
 * note: strlist_pop does not release the memory allocated for element
 */
static inline char *strlist_pop(struct strlist *sl)
{
	return strlist_pop2(sl, 1);
}

/**
 * strlist_dump2 - dump elements into array
 *
 * copy: if this value is 0, the elements are not copied and the dumped array
 *	 stores the pointer points to the elements. Otherwise, each element is
 *	 copied.
 */
char **strlist_dump2(struct strlist *sl, int copy);

/**
 * strlist_dump2 - dump elements into array
 */
static inline char **strlist_dump(struct strlist *sl)
{
	return strlist_dump2(sl, 1);
}

/*
 * strlist_split_word - split string, make sure the words don't get broken
 *			into pieces
 */
void strlist_split_word(struct strlist *sl, const char *str, uint bound);

#endif /* STRLIST_H */
