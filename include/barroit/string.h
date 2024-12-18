/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef BRT_STRING_H
#define BRT_STRING_H

/**
 * strrepl - Replace all occurrences of ‘c’ in ‘str’ with ‘r’.
 */
void strrepl(char *str, int c, int r);

/**
 * straftr - Get the position of next character after skipping ‘pref’ at the
 * 	     beginning of ‘str’.
 *
 * return  - The pointer to the next character. Otherwise, NULL is returned.
 */
char *straftr(const char *str, const char *pref);

/**
 * strskip - Advance the pointer 'str' past the 'pref'
 *
 * return  - 0 on success, -1 if 'pref' is not a prefix of 'str'
 */
int strskip(const char *str, const char *pref, const char **rest);

/**
 * strskipws - Skip leading whitespaces
 */
char *strskipws(const char *str);

/**
 * strtrimend - Remove tailing whitespaces
 */
char *strtrimend(char *str);

/**
 * strnxtws - Locate whitespace character
 *
 * return - pointer to the whitespace character, NULL if not found
 */
char *strnxtws(const char *str);

int str2ullong(const char *str, size_t len, ullong *value, ullong max);

int str2llong(const char *str, size_t len, llong *value, llong min, llong max);

static inline int str2u32(const char *str, u32 *value)
{
	ullong val;
	int ret = str2ullong(str, -1, &val, UINT32_MAX);
	if (!ret)
		*value = val;
	return ret;
}

static inline int str2u8(const char *str, u8 *value)
{
	ullong val;
	int ret = str2ullong(str, -1, &val, UINT8_MAX);
	if (!ret)
		*value = val;
	return ret;
}

#if UINT_MAX != UINT32_MAX
# error "BUG! str2uint str2u32"
#endif
#define str2uint str2u32

int str2timespan(const char *str, u32 *val);

int str2bool(const char *str, int *val);

#endif /* BRT_STRING_H */
