// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "win/ui.hpp"
#include "win/termsg.hpp"
#include "barroit/io.h"
#include "runopt.h"
#include "getconf.h"

static struct cmdarg args;
static struct savesave savconf;

static int argc;
static char **argv;

static void validate_os_version()
{
	if (!IsWindows7OrGreater())
		die("unsupported windows version (at least win7)");
}

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	int err;

	setup_console();
	validate_os_version();

	argc = cmdline2argv(cmdline, &argv);
	if (argc > 1) {
		err = parse_option(argc, argv, &args);
		EXIT_ON(err);
	}

	if (args.output) {
		teardown_console();
		redirect_output(args.output);
	} else {
		// hide_console();
	}

	err = parse_savesave_config(args.confpath, &savconf);
	EXIT_ON(err);

	HWND window;
	err = create_app_window(app, &window);
	EXIT_ON(err);

	NOTIFYICONDATA icon;
	setup_notifyicon(app, window, &icon);

	show_notifyicon(&icon);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}

bool is_console_output()
{
	return !args.output;
}
