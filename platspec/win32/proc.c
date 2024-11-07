// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "termas.h"
#include "command.h"
#include "path.h"
#include "strbuf.h"

int pid_is_alive(pid_t pid)
{
	HANDLE proc = OpenProcess(SYNCHRONIZE, FALSE, pid);
	if (proc) {
		CloseHandle(proc);
		return 1;
	}

	if (GetLastError() == ERROR_INVALID_PARAMETER)
		return 0;

	die_winerr(_("failed to determine existence of process `%d'"), pid);
}

int pid_kill(pid_t pid, int sig)
{
	int ret = 0;
	HANDLE p = OpenProcess(PROCESS_TERMINATE, 0, pid);
	if (!p)
		goto err_out;

	int err = !TerminateProcess(p, 0);
	if (err)
		goto err_out;

	if (0) {
	err_out:
		errno = EPERM;
		ret = -1;
	}

	CloseHandle(p);
	return ret;
}

int proc_rd_io(const char *name, flag_t flags)
{
	DWORD af = 0;
	DWORD sf = 0;

	if (flags & PROC_RD_STDIN) {
		af |= GENERIC_READ;
		sf |= FILE_SHARE_READ;
	}

	if (flags & (PROC_RD_STDOUT | PROC_RD_STDERR)) {
		af |= GENERIC_WRITE;
		sf |= FILE_SHARE_WRITE;
	}

	SECURITY_ATTRIBUTES sec = {
		.nLength        = sizeof(sec),
		.bInheritHandle = 1,
	};
	HANDLE file = CreateFile(name, af, sf, &sec,
				 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE) {
		errno = last_errno_error();
		return -1;
	}
	SetFilePointer(file, 0, NULL, FILE_END);

	int err;
	if (flags & PROC_RD_STDIN) {
		err = !SetStdHandle(STD_INPUT_HANDLE, file);
		if (err) {
			err = PERR_RD_STDIN;
			goto err_out;
		}
	}

	if (flags & PROC_RD_STDOUT) {
		err = !SetStdHandle(STD_OUTPUT_HANDLE, file);
		if (err) {
			err = PERR_RD_STDOUT;
			goto err_out;
		}
	}

	if (flags & PROC_RD_STDERR) {
		err = !SetStdHandle(STD_ERROR_HANDLE, file);
		if (err) {
			err = PERR_RD_STDERR;
			goto err_out;
		}
	}

	CloseHandle(file);

	err = __proc_rd_io(name, flags);
	if (!err) {
		if (flags & PROC_RD_STDIN)
			setvbuf(stdin, NULL, _IONBF, 0);
		if (flags & PROC_RD_STDOUT)
			setvbuf(stdout, NULL, _IONBF, 0);
		if (flags & PROC_RD_STDERR)
			setvbuf(stderr, NULL, _IONBF, 0);
	}

	return err ? : 0;

err_out:
	errno = last_errno_error();
	CloseHandle(file);
	return err;
}

#ifndef proc_detach
void proc_detach(void)
{
# ifndef CONFIG_NO_WIN_GUI
	setenv(PROC_DO_MINIMAL, "y", 1);
# endif
}
#endif

int proc_exec(struct proc *proc, const char *file, ...)
{
	va_list ap;
	struct strbuf sb = STRBUF_INIT;
	STARTUPINFO info = {
		.cb      = sizeof(info),
	};

	va_start(ap, file);

	const char *arg;
	while ((arg = va_arg(ap, typeof(arg))) != NULL) {
		int need_quote = !!strnxtws(arg);
		if (!need_quote)
			strbuf_concat(&sb, arg);
		else
			strbuf_printf(&sb, "\"%s\"", arg);

		strbuf_concat_char(&sb, ' ');
	}

	va_end(ap);

	/*
	 * Why the fuck is CreateProcess designed so sucky?
	 * And why the fuck do stdio handles get inherited even when
	 * bInheritHandles is set to 0?
	 * What the fuck is this piece of shit.
	 */
	int ret = !CreateProcess(file, sb.str, NULL, NULL, 0,
				 0, NULL, NULL, &info, &proc->info);
	if (ret)
		warn_winerr(_("failed to create process for program `%s'"),
			    file);

	strbuf_destroy(&sb);
	return ret;
}

static int proc_exit_code(struct proc *proc, int *ret)
{
	DWORD code;
	int err = !GetExitCodeProcess(proc->pc_proc, &code);
	if (err)
		return warn_winerr(_("failed to get exit code for process `%d'"),
				   proc->pc_pid);

	*ret = code;
	return 0;
}

int proc_wait(struct proc *proc, int *__ret)
{
	int ret = WaitForSingleObject(proc->pc_proc, INFINITE);
	switch (ret) {
	case WAIT_OBJECT_0:
		ret = proc_exit_code(proc, __ret);
		break;
	case WAIT_FAILED:
		ret = warn_winerr(ERRMAS_WAIT_PROC(proc->pc_pid));
	}

	CloseHandle(proc->pc_proc);
	CloseHandle(proc->pc_thrd);
	return ret;
}
