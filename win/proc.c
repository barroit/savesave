// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"

int PLATSPECOF(is_process_alive)(ulong pid)
{
	HANDLE proc = OpenProcess(SYNCHRONIZE, FALSE, pid);
	if (!proc)
		return 0;

	CloseHandle(proc);
	return 1;
}
