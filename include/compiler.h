// SPDX-License-Identifier: GPL-3.0
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

#define __format(sta, fir) __attribute__((format(printf, sta, fir)))

#define NORETURN void __attribute__((noreturn))

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H */
