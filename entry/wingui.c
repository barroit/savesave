// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "atenter.h"
#include "console.h"
#include "getargv.h"

extern int cmd_main(int argc, const char **argv);
extern void (*setup_longrunning_command)(void);

#include "list.h"

int WinMain(HINSTANCE app, HINSTANCE _, char *cmdline, int ___)
{
	setup_program();
	setup_console();
	post_setup_program();

	const char *defargv[] = { "savesave", "start" };
	int argc = sizeof_array(defargv);
	const char **argv = defargv;

	if (*cmdline)
		argc = getargv(&argv);

	setup_longrunning_command = NULL;
	cmd_main(argc, argv);

	exit(39);
}
