/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PROC_H
#define PROC_H

/*
 * Find process id of savesave
 */
pid_t savesave_pid(void);

/*
 * Assert there is only one process of savesave running on machine.
 */
void assert_unic_proc(void);

/*
 * Dump process id to pid file
 */
void pid_dump(void);

/*
 * Remove pid file
 */
void pid_erase(void);

int proc_is_alive(pid_t pid);

#ifdef CONFIG_NO_LONGRUNNING_DAEMON
# define proc_detach() do {} while (0)
#else
void proc_detach(void);
#endif

#ifdef __unix__
# define proc_kill kill
#else
void proc_kill(pid_t pid, int sig);
#endif

#endif /* PROC_H */
