// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "atenter.h"
#include "maincmd.h"
#include "proc.h"

void check_unique_process(void);

int main(int argc, const char **argv)
{
	do_setup();
	do_delayed_setup();

	setup_message_translation();

	cmd_main(argc, argv);

	if (!is_longrunning)
		exit(0);

	push_process_id();
	atexit(pop_process_id);

	exit(0);
}
