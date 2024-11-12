/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef TRACE_H
#define TRACE_H

void trace_start_timer(void);

void trace_stop_timer(void);

void __trace_print_timer(const char *name);

#define trace_print_timer() __trace_print_timer(__PRETTY_FUNCTION__)

#endif /* TRACE_H */
