// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "windows.h"
#include "win/console.hpp"
#include "runopt.h"
#include <stdio.h>

static void handle_command_line(const char *cmdline)
{
	setup_console();

	int argc, res;
	char **argv;

	argc = cmdline2argv(cmdline, &argv);
	res = parse_option(argc, argv);

	if (res == OPT_HELP || res == OPT_VERSION) {
		putchar('\n');
		waiting_user();
		exit(0);
	}

	rmargv(argc, argv);
}

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	if (*cmdline)
		handle_command_line(cmdline);

	return 0;
}
