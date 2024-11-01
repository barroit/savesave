// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "command.h"
#include "i18n.h"
#include "prog.h"
#include "proc.h"
#include "termas.h"

int main(int argc, const char **argv)
{
	prog_init();
	prog_init_ob();

	i18n_setup();

	cmd_main(argc, argv);

	HANDLE thread = GetCurrentThread();
	WaitForSingleObject(thread, INFINITE);
}
