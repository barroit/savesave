// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "win/ui.hpp"
#include "win/termsg.hpp"
#include "barroit/io.h"
#include "win/cmdline.hpp"
#include "win/backup.hpp"
#include "savconf.h"
#include "debug.h"

static class console *console_reference;

static void check_os_version()
{
	if (!IsWindows7OrGreater())
		die("unsupported windows version (at least win7)");
}

static void init_ui_component(HINSTANCE app)
{
	int err;

	HWND window;
	err = create_app_window(app, &window);
	if (err)
		exit(128);

	NOTIFYICONDATA icon;
	setup_notifyicon(app, window, &icon);

	show_notifyicon(&icon);
}

static void loop_ui_message()
{
	MSG message;
	while (GetMessage(&message, NULL, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	console con;
	con.setup_console();
	console_reference = &con;

	check_os_version();

	uarg_parser parser;
	parser.dump_cmdline(cmdline);

	parser.parse_cmdline();
	if (!con.update_stdio_on(parser.args.output))
		con.hide_console();

	parser.parse_savconf();
	DEBUG_RUN(print_savconf, parser.savconf, parser.nconf);

	backup bu{ parser.nconf };
	bu.create_backup_task(parser.savconf);

	init_ui_component(app);
	loop_ui_message();

	return 0;
}

class console *get_console()
{
	return console_reference;
}
