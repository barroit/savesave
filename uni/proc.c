// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "termas.h"

int PLATSPECOF(is_process_alive)(ulong pid)
{
	int err;

	err = kill(pid, 0);
	if (!err)
		return 1;
	else if (errno == ESRCH)
		return 0;

	die_errno(_("failed to determine existence of process `%lu'"), pid);
}
