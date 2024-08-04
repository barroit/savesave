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
#include "win/notifyicon.hpp"
#include "win/termsg.hpp"
#include "win/window.hpp"

static struct cmdarg args;
static struct savesave savconf;

static void handle_command_line_option(const char *cmdline)
{
	int err;

	char **argv;
	int argc = cmdline2argv(cmdline, &argv);

	err = parse_option(argc, argv, &args);
	EXIT_ON(err);

	rmargv(argc, argv);
}

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

	if (*cmdline)
		handle_command_line_option(cmdline);

	err = parse_savesave_config(args.confpath, &savconf);
	EXIT_ON(err);

	// err = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// BUG_ON(err);

	// HWND window;
	// err = create_app_window(app, &window);
	// EXIT_ON(err);

	// ShowWindow(window, SW_HIDE);

	// MSG message;
	// while (GetMessage(&message, NULL, 0, 0) > 0) {
	// 	TranslateMessage(&message);
	// 	DispatchMessage(&message);
	// }

	// pass = ShowWindow(window, SW_HIDE);
	// printf("%d\n", pass);
	// BUG_ON(!pass);

	return 0;
}
