// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef TERMSG_H
#define TERMSG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include <errno.h>

int error_routine(const char *extr, const char *fmt, ...) __format(2, 3);

NORETURN die_routine(const char *extr, const char *fmt, ...) __format(2, 3);

NORETURN bug_routine(const char *file,
		     int line, const char *fmt, ...) __format(3, 4);

extern char *strerror(int);

#define error(...)       error_routine(0, __VA_ARGS__)
#define error_errno(...) error_routine(strerror(errno), __VA_ARGS__)

#define die(...)       die_routine(0, __VA_ARGS__)
#define die_errno(...) die_routine(strerror(errno), __VA_ARGS__)

#define bug(...) bug_routine(__FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* TERMSG_H */

