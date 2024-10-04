// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "termas.h"

int PLATSPECOF(is_process_alive)(pid_t pid)
{
	int err;

	err = kill(pid, 0);
	if (!err)
		return 1;
	else if (errno == ESRCH)
		return 0;

	die_errno(_("failed to determine existence of process `%d'"), pid);
}

void detach_process(void)
{
	int err = daemon(1, 1);
	if (!err)
		return;

	warn_errno(_("failed to detach process from controlling terminal"));
}
