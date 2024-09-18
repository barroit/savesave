/* SPDX-License-Identifier: GPL-3.0-only */
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

#define fix_grow(x) (((x) + 16) * 3 / 2)

#define bitsizeof(x) (CHAR_BIT * sizeof(x))

#define max_uint_valueof(x) \
	(UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(x)))

#define max_int_valueof(x) \
	(INTMAX_MAX >> (bitsizeof(intmax_t) - bitsizeof(x)))

#define uint_mult_overflows(a, b) \
	((a) && ((b) > (max_uint_valueof(a) / (a))))

#define uint_add_overflows(a, b) \
    ((b) > max_uint_valueof(a) - (a))

extern NORETURN __die_routine(const char *, const char *, const char *, ...);

#define st_mult(a, b)							\
({									\
	if (unlikely(uint_mult_overflows(a, b))) {			\
		__die_routine(_("fatal: "), NULL, _("size overflow: "	\
			      "%" PRIuMAX " * %" PRIuMAX ""),		\
			      (uintmax_t)(a), (uintmax_t)(b));		\
	}								\
	((a) * (b));							\
})

#define st_add(a, b)							\
({									\
	if (unlikely(uint_add_overflows(a, b))) {			\
		__die_routine(_("fatal: "), NULL, _("size overflow: "	\
			      "%" PRIuMAX " + %" PRIuMAX ""),		\
			      (uintmax_t)(a), (uintmax_t)(b));		\
	}								\
	((a) + (b));							\
})

#ifdef __cplusplus
}
#endif

#endif /* CALC_H */
