/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PROC_H
#define PROC_H

void assert_unique_process(void);

int PLATSPECOF(is_process_alive)(pid_t pid);

void push_process_id(void);

void pop_process_id(void);

void detach_process(void);

#endif /* PROC_H */
