/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PROC_H
#define PROC_H

/*
 * Dump process id to pid file
 */
void pid_dump(void);

/*
 * Find process id of savesave
 */
pid_t pid_retrieve(void);

/*
 * Remove pid file
 */
void pid_erase(void);

/*
 * Check if the process associated with given pid is running.
 */
int pid_is_alive(pid_t pid);

/*
 * Kill a process by pid.
 */
#ifdef __unix__
# define pid_kill kill
#else
int pid_kill(pid_t pid, int sig);
#endif

/*
 * Assert there is only one process of savesave running on machine.
 */
void proc_assert_unique(void);

/*
 * Detach self from terminal.
 */
#ifdef CONFIG_NO_LONGRUNNING_DAEMON
# define proc_detach() do {} while (0)
#else
void proc_detach(void);
#endif

struct proc {
#ifdef __unix__
	pid_t pid;
#elif defined(_WIN32)
	HANDLE handle;
#endif
};

int proc_exec(struct proc **proc, const char *file, ...);

int proc_wait(struct proc *proc);

#ifdef __unix__
static inline int proc_kill(struct proc *proc, int sig)
{
	return kill(proc->pid, sig);
}
#else
int proc_kill(struct proc *proc, int sig);
#endif

#endif /* PROC_H */
