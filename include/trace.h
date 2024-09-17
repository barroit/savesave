/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef TRACE_H
#define TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

void trace_start_timer(void);

void trace_stop_timer(void);

void __trace_print_timer(const char *name);

#define trace_print_timer() __trace_print_timer(__PRETTY_FUNCTION__)

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */