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
#include "win/notifarea.hpp"
#include "win/termsg.hpp"
#include "win/window.hpp"

static const char *savconf_path;
static struct savesave savconf;

static void handle_command_line_option(const char *cmdline)
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

static void validate_os_version()
{
	if (!IsWindows7OrGreater())
		die("unsupported windows version (at least win7)");
}

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	int err, pass;

	setup_console();
	validate_os_version();

	if (*cmdline)
		handle_command_line_option(cmdline);

	err = parse_savesave_config(savconf_path, &savconf);
	EXIT_ON(err);

	HWND window;
	err = create_app_window(app, &window);
	EXIT_ON(err);

	pass = ShowWindow(window, SW_HIDE);
	BUG_ON(!pass);

	return 0;
}
