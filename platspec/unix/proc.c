// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "proc.h"
#include "termas.h"
#include "alloc.h"
#include "robio.h"
#include "strlist.h"

int pid_is_alive(pid_t pid)
{
	int err;

	err = kill(pid, 0);
	if (!err)
		return 1;
	else if (errno == ESRCH)
		return 0;

	die_errno(_("failed to determine existence of process `%d'"), pid);
}

#ifndef proc_detach
void proc_detach(void)
{
	if (!cm_io_need_update) {
		const char *output = output_path();
		cm_no_detach = termas_rd_output(output);
	}

	if (cm_no_detach)
		return;

	int err = daemon(1, cm_io_need_update);
	if (err)
		warn_errno(_("failed to detach process from controlling terminal"));
}
#endif

enum cerr_type {
	CERR_EXEC,
	CERR_DUP2,
	CERR_SIZE,
};

struct cerr {
	enum cerr_type type;
	int errnum;

	size_t size;
	char info[];
};

static void warn_cerr_exec(int fd, struct cerr *err)
{
	char *buf = xmalloc(err->size);
	ssize_t nr = robread(fd, buf, err->size);
	if (nr < 0) {
		free(buf);
		buf = NULL;
	}

	errno = err->errnum;
	if (buf)
		warn_errno(_("failed to execute `%s'"), buf);
	else
		warn_errno(_("failed to execute program"));
	free(buf);
}

static void warn_cerr_dup2(int fd, struct cerr *err)
{
	int errfd;
	ssize_t nr = robread(fd, &errfd, err->size);
	if (nr < 0)
		errfd = -1;

	const char *strio;
	switch (errfd) {
	case STDIN_FILENO:
		strio = "stdin";
		break;
	case STDOUT_FILENO:
		strio = "stdout";
		break;
	case STDERR_FILENO:
		strio = "stderr";
		break;
	default:
		strio = "stdio";
	}

	errno = err->errnum;
	warn_errno(_("failed to redirect %s to null device"), strio);
}

static int warn_cerr(struct proc *proc, int fd, struct cerr *err)
{
	int ret;
	typeof(warn_cerr_exec) *cbs[CERR_SIZE] = {
		[CERR_EXEC] = warn_cerr_exec,
		[CERR_DUP2] = warn_cerr_dup2,
	};

	cbs[err->type](fd, err);
	proc_wait(proc, &ret);
	return ret;
}

static __noreturn void die_cerr(int fd, enum cerr_type type,
				const void *info, size_t size)
{
	struct cerr *err = xmalloc(sizeof(*err) + size);

	err->type = type;
	err->errnum = errno;

	err->size = size;
	memcpy(err->info, info, size);

	robwrite(fd, err, sizeof(*err) + size);
	_exit(-1);
}

static char **ap_to_argv(va_list ap)
{
	struct strlist sl;
	strlist_init(&sl, STRLIST_STORE_REF);

	while (39) {
		const char *arg = va_arg(ap, const char *);
		if (arg == NULL)
			break;
		strlist_push(&sl, arg);
	}

	char **argv = strlist_dump2(&sl, 0);
	strlist_destroy(&sl);
	return argv;
}

static void cloexec(int fd)
{
	int flags = fcntl(fd, F_GETFD);
	fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

int proc_exec(struct proc *proc, const char *file, ...)
{
	int pp[2];
	int ret = pipe(pp);
	if (ret) {
		warn_errno(_("failed to make pipe for processes"));
		memset(pp, ~0, sizeof(pp));
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(pp[0]);
		close(pp[1]);
		return warn_errno(_("failed to create new process"));
	}

	if (pid != 0) {
		close(pp[1]);
		proc->pid = pid;

		struct cerr err;
		ssize_t nr = read(pp[0], &err, sizeof(err));
		if (nr)
			ret = warn_cerr(proc, pp[0], &err);

		close(pp[0]);
		return ret;
	} else {
		close(pp[0]);
		if (proc_use_io_rd(proc->flags)) {
			ret = proc_rd_io(NULL_DEVICE, proc->flags);
			if (ret) {
				ret = PERR_RD_ERR(ret);
				die_cerr(pp[1], CERR_DUP2, &ret, sizeof(ret));
			}
		}

		va_list ap;
		va_start(ap, file);

		char **argv = ap_to_argv(ap);
		va_end(ap);

		cloexec(pp[1]);
		execvp(file, argv);

		size_t len = strlen(file);
		die_cerr(pp[1], CERR_EXEC, file, len + 1);
	}
}

int proc_wait(struct proc *proc, int *ret)
{
	pid_t pid;
	int ws;

retry:
	pid = waitpid(proc->pid, &ws, 0);
	if (pid == -1) {
		if (errno != EINTR)
			return warn_errno(ERRMAS_WAIT_PROC(pid));
		goto retry;
	}

	if (WIFEXITED(ws))
		*ret = WEXITSTATUS(ws);
	else if (WIFSIGNALED(ws))
		*ret = WTERMSIG(ws);
	return 0;
}
