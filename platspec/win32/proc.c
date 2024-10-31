// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "termas.h"
#include "command.h"

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
	return 0;
}

#ifndef proc_detach
void proc_detach(void)
{
# ifdef CONFIG_NO_WIN_GUI
	if (!cm_io_need_update) {
		int err = redirect_output(NULL_DEVICE);
		if (err)
			return;
	}

	FreeConsole();
# endif
}
#endif

int proc_exec(struct proc *proc, const char *file, ...)
{
	return 0;
}

int proc_wait(struct proc *proc, int *ret)
{
	return 0;
}
