/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_STRING_H
#define BRT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * strrepl - Replace all occurrences of ‘c’ in ‘str’ with ‘r’.
 */
void strrepl(char *str, int c, int r);

/**
 * straftr - Get the position of next character after skipping ‘pref’ at the
 * 	     beginning of ‘str’.
 *
 * return  - the pointer to the next character. Otherwise, NULL is returned.
 */
const char *straftr(const char *str, const char *pref);

#ifdef __cplusplus
}
#endif

#endif /* BRT_STRING_H */
