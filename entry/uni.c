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
#include "atexit.h"

static void __prepare_longrunning(void)
{
	detach_process();

	setup_lr_logging();
	atexit_enque(teardown_lr_logging);

	push_process_id();
	atexit_enque(pop_process_id);

	atexit_apply();
}

int main(int argc, const char **argv)
{
	setup_message_translation();

	prepare_longrunning = __prepare_longrunning;
	cmd_main(argc, argv);

	if (!is_longrunning)
		exit(0);

	while (39)
		pause();
}
