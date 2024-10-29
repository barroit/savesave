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
	flag_t flags;

#ifdef __unix__
	pid_t pid;
#elif defined(_WIN32)
	HANDLE handle;
#endif
};

#define PROC_INIT { 0 };

#define PROC_RD_STDIN  (1 << 0)
#define PROC_RD_STDOUT (1 << 1)
#define PROC_RD_STDERR (1 << 2)

int proc_exec(struct proc *proc, const char *file, ...);

int proc_wait(struct proc *proc);

#define PERR_RD_OFFSET 39
#define PERR_RD_STDIN  (-(STDIN_FILENO + PERR_RD_OFFSET))
#define PERR_RD_STDOUT (-(STDOUT_FILENO + PERR_RD_OFFSET))
#define PERR_RD_STDERR (-(STDERR_FILENO + PERR_RD_OFFSET))

/*
 * Redirect stdout/stderr/stdin to specified file.
 */
int proc_rd_io(const char *name, flag_t flags);

#ifdef __unix__
static inline int proc_kill(struct proc *proc, int sig)
{
	return kill(proc->pid, sig);
}
#else
int proc_kill(struct proc *proc, int sig);
#endif

#endif /* PROC_H */
