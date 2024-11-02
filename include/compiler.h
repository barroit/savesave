/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef NDEBUG
# define BUG_ON(cond) do { if (cond); } while (0)
#else
# define BUG_ON(cond) assert(!(cond))
#endif

#define __format(start, first) __attribute__((format(printf, start, first)))

#ifdef CONFIG_CC_IS_GCC
# define __noreturn __attribute__((noreturn))
#else
# define __noreturn _Noreturn
#endif

/*
 * I'm fucking using __, no matter what!
 */
#ifdef __nonnull
# undef __nonnull
#endif

#define __nonnull(...) \
	__attribute__((nonnull(__VA_ARGS__)))

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

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

#define typeof_member(type, memb) typeof(((type *)0)->memb)

#define sizeof_array(arr) (sizeof(arr) / sizeof((arr)[0]))

#define same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#ifndef static_assert
# define static_assert(expr, message) _Static_assert(expr, message)
#endif

#define containerof(ptr, type, memb)					\
({									\
	static_assert(same_type(*(ptr), typeof_member(type, memb)) ||	\
		      same_type(*(ptr), void),				\
		      "pointer type mismatch in containerof()");	\
	((type *)(((void *)ptr) - offsetof(type, memb)));		\
})

/*
 * Returns the number of arguments (up to 6) passed to this macro. Useful for
 * implementing flexible function calls.
 */
#define get_arg_cnt(...) __get_arg_cnt(, ##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
#define __get_arg_cnt(...) ___get_arg_cnt(__VA_ARGS__)
#define ___get_arg_cnt(a1, a2, a3, a4, a5, a6, a7, cnt, ...) cnt

/*
 * Another version of syscall(2). This version does not convert the return
 * value to errno if an error occurs.
 */
#define __syscall(number, ...) \
	___syscall(get_arg_cnt(__VA_ARGS__), number, ##__VA_ARGS__)
#define ___syscall(...) \
	____syscall(__VA_ARGS__)
#define ____syscall(count, number, ...) \
	__syscall##count(number, ##__VA_ARGS__)

/* glibc/sysdeps/unix/sysv/linux/x86_64/sysdep.h */
/*
 * This also works when v is an array. For an array v, type of (v) - (v) is
 * ptrdiff_t, which is signed, since size of ptrdiff_t == size of pointer, cast
 * is a NOP.
 */
#define TYPEFY(v) typeof((v) - (v))

/* glibc/sysdeps/unix/sysv/linux/x86_64/sysdep.h */
/*
 * Explicit cast the argument.
 */
#define ARGIFY(v) ((TYPEFY (v)) (v))

/* glibc/sysdeps/unix/sysv/linux/x86_64/sysdep.h */
/*
 * Create a variable 'name' based on type of variable 'v' to avoid explicit
 * types.
 */
#define VARAFY(v, name) typeof(ARGIFY(v)) name

#define __cold __attribute__((__cold__))

#if defined(__unix__)
# define __FEATSPEC_NAME uni
#elif defined(_WIN32)
# define __FEATSPEC_NAME win
#endif

#define FEATSPEC(x)       __FEATSPEC(__FEATSPEC_NAME, x)
#define __FEATSPEC(p, x)  ___FEATSPEC(p, x)
#define ___FEATSPEC(p, x) __feat_##p##_##x

#endif /* COMPILER_H */
