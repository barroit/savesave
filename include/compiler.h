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
# define CONSTRUCTOR void
#else
# define CONSTRUCTOR static void __attribute__((constructor))
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H */
