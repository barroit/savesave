/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PROC_H
#define PROC_H

pid_t check_unique_process(char **name, int abort);

int process_is_alive(pid_t pid);

void push_process_id(void);

void pop_process_id(void);

void detach_process(void);

void kill_process(pid_t pid, int signal);

#ifdef __unix__
#define kill_process kill
#endif

#endif /* PROC_H */
