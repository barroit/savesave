// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "termas.h"

int proc_is_alive(pid_t pid)
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
