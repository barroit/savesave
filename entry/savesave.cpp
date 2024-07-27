// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "barroit/io.h"
#include "runopt.h"
#include "getconf.h"
#include "termsg.h"

static const char *savconf_path;
static struct savesave savconf;

static void handle_command_line(const char *cmdline)
{
	char **argv;
	int argc = cmdline2argv(cmdline, &argv);
	enum optid res = parse_option(argc, argv);

	if (res == OPT_HELP || res == OPT_VERSION)
		exit(128);
	else if (res == OPT_CONFIG)
		savconf_path = optarg;

	rmargv(argc, argv);
}

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	int err;

	setup_console();
	
	if (!IsWindows7OrGreater()) {
		error("unsupported windows version (at least win7)");
		exit(128);
	}

	// co init here

	if (*cmdline)
		handle_command_line(cmdline);

	err = parse_savesave_config(savconf_path, &savconf);
	if (err)
		exit(128);

	return 0;
}
