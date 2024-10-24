/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CALC_H
#define CALC_H

#define fix_grow(x) (((x) + 16) * 3 / 2)

#define bit_sizeof(x) (CHAR_BIT * sizeof(x))

#define value_is_signed(x) ((typeof(x))-1 < 0)

#define offsetof_intmax(x) (bit_sizeof(uintmax_t) - bit_sizeof(x))

#define max_value(x) \
	((UINTMAX_MAX >> value_is_signed(x)) >> offsetof_intmax(x))

#define umult_is_overflow(a, b) ((a) && (b) > (max_value(a) / (a)))

#define uadd_is_overflow(a, b) ((b) > (max_value(a) - (a)))

void __noreturn ___die_ucalc_overflow(const char *file, int line,
				      uintmax_t a, uintmax_t b,
				      int op, uint size);

#define __die_ucalc_overflow(a, b, op) \
	___die_ucalc_overflow(__FILE__, __LINE__, a, b, op, sizeof(a));

#define st_umult(a, b)					\
({							\
	if (unlikely(umult_is_overflow(a, b)))		\
		__die_ucalc_overflow(a, b, '*');	\
	(a) * (b);					\
})

#define st_uadd(a, b)					\
({							\
	if (unlikely(uadd_is_overflow(a, b)))		\
		__die_ucalc_overflow(a, b, '+');	\
	(a) + (b);					\
})

/*
 * Do not perform operations that produce side effects
 */
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define sec_to_millisec(x) st_umult(x, 1000)
#define sec_to_microsec(x) st_umult(sec_to_millisec(x), 1000)
#define sec_to_nanosec(x)  st_umult(sec_to_microsec(x), 1000)

#endif /* CALC_H */
