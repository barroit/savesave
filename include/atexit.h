/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * not necessary for current work, leave definitions
 * here for later implementation
 */

#ifndef ATEXIT_H
#define ATEXIT_H

typedef void (*atexit_function_t)(void);

void atexit_enque(atexit_function_t func);

atexit_function_t atexit_deque(void);

void atexit_apply(void);

#endif /* ATEXIT_H */
