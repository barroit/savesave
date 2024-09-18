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

#define CONCAT2(a, b)	__CONCAT2(a, b)
#define __CONCAT2(a, b)	a##b

/*
 * Call function by arguments count; tailing number indicates the max arguments
 * it can handle
 * 
 * A piece of shit
 */
#define __SELECT_FUNCTION3(a, b, c, n, ...)	n
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

#define IS_DEFINED(x)      __IS_DEFINED(x)
#define __IS_DEFINED(val)  ___IS_DEFINED(__ARG_PLACEHOLDER_##val)
#define ___IS_DEFINED(arg) __take_second_arg(arg 1, 0)

#ifdef __linux
# define __PLATSPEC_NAME uni
#else
# define __PLATSPEC_NAME win
#endif

#define PLATSPECOF(name)	 __PLATSPECOF(__PLATSPEC_NAME, name)
#define __PLATSPECOF(suf, name)  ___PLATSPECOF(suf, name)
#define ___PLATSPECOF(suf, name) __platspec_##suf##_##name

#define LONGRUNNING
#define USESTDIO

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H */
