/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#define FORMAT(sta, fir) __attribute__((format(printf, sta, fir)))

#define NORETURN void __attribute__((noreturn))

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifdef _WIN32
/*
 * __attribute__((constructor)) just does not work on windows
 */
# define CONSTRUCTOR(name) void name(void)
#else
# define CONSTRUCTOR(name) static void __attribute__((constructor)) name(void)
#endif

#define __CONCAT2(a, b)	a##b
#define CONCAT2(a, b)	__CONCAT2(a, b)

#define UNIQUE_ID(n) CONCAT2(CONCAT2(__local_, n), __COUNTER__)

#define __SELECT_FUNCTION3(a, b, c, n, ...)	n

/*
 * Call function by arguments count; tailing number indicates the max arguments
 * it can handle
 */
#define FLEXCALL_FUNCTION3(prefix, ...)				\
	__SELECT_FUNCTION3(__VA_ARGS__,				\
			   CONCAT2(prefix, 3),			\
			   CONCAT2(prefix, 2),			\
			   CONCAT2(prefix, 1))(__VA_ARGS__)

#define typeof_member(type, member) typeof(((type *)0)->member)

#define sizeof_array(arr) (sizeof(arr) / sizeof((arr)[0]))

#define same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#ifndef static_assert
# define static_assert(expr, message) _Static_assert(expr, message)
#endif

#define containerof(ptr, type, member)					\
({									\
	static_assert(same_type(*(ptr), typeof_member(type, member)) ||	\
		      same_type(*(ptr), void),				\
		      _("pointer type mismatch in containerof()"));	\
	((type *)(((void *)ptr) - offsetof(type, member)));		\
})

#define __ARG_PLACEHOLDER_1 0,
#define __take_second_arg(fir, sec, ...) sec

#define ___IS_DEFINED(arg) __take_second_arg(arg 1, 0)
#define __IS_DEFINED(val)  ___IS_DEFINED(__ARG_PLACEHOLDER_##val)
#define IS_DEFINED(x)      __IS_DEFINED(x)

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H */
