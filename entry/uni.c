// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "maincmd.h"
#include "termas.h"
#include "proc.h"
#include "path.h"

static void __prepare_longrunning(void)
{
	// make_daemon();

	const char *iodest = get_log_filename();
	redirect_output(iodest);
	/* todo: atexit cleanup empty log */

	push_process_id();
	atexit(pop_process_id);
}

int main(int argc, const char **argv)
{
	setup_message_translation();

	prepare_longrunning = __prepare_longrunning;
	cmd_main(argc, argv);

	exit(0);
}
