// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "maincmd.h"
#include "proc.h"

int main(int argc, const char **argv)
{
	setup_message_translation();

	cmd_main(argc, argv);

	if (!is_longrunning)
		exit(0);

	push_process_id();
	atexit(pop_process_id);

	pthread_exit(NULL);
}
