// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "maincmd.h"
#include "proc.h"

static void __prepare_longrunning(void)
{
	// make_daemon();

	push_process_id();
	atexit(pop_process_id);
}

int main(int argc, const char **argv)
{
	setup_message_translation();

	prepare_longrunning = __prepare_longrunning;
	cmd_main(argc, argv);
}
