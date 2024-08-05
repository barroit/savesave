// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CALC_H
#define CALC_H

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_SIZEOF(arr) (sizeof(arr) / sizeof((arr)[0]))

#define fix_grow(x) ((x + 16) * 3 / 2)

#define bitsizeof(x)    (CHAR_BIT * sizeof(x))

#define max_uint_val(x) (UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(x)))

#define uint_mult_overflows(a, b) ((a) && ((b) > (max_uint_val(a) / (a))))

extern NORETURN __die_routine(const char *, const char *, const char *, ...);

#define st_mult(a, b)							\
	({								\
		if (uint_mult_overflows(a, b)) {			\
			__die_routine("fatal: ", NULL, "size overflow: "\
				      "%" PRIuMAX " * %" PRIuMAX ,	\
				      (uintmax_t)a, (uintmax_t)b);	\
		}							\
		a * b;							\
	})

#ifdef __cplusplus
}
#endif

#endif /* CALC_H */
