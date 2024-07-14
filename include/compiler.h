// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef COMPILER_H
#define COMPILER_H

#define __format(sta, fir) __attribute__((format(printf, sta, fir)))

#define NORETURN void __attribute__((noreturn))

#endif /* COMPILER_H */
