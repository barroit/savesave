// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "win/ui.hpp"
#include "termas.h"
#include "robio.h"
#include "win/cmdline.hpp"
#include "win/backup.hpp"
#include "dotsav.h"
#include "debug.h"
#include "win/atenter.hpp"
#include "runopt.h"
#include "strlist.h"

static class console appcon;
static struct cmdarg args;
static struct savesave *dotsav;

#ifdef CONFIG_IS_CONSOLE_APP
int main(int argc, char **argv)
#else
int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
#endif
{
	/*
	 * class constructor is guaranteed to run before main function, use
	 * that for constructor function leak support on windows
	 */
	static class atenter constructor;

#ifndef CONFIG_IS_CONSOLE_APP
	appcon.setup_console();
#endif
	setup_console_codepage();
	setup_message_translation();

	constructor.precheck();

#ifndef CONFIG_IS_CONSOLE_APP
	char **argv;
	int argc = cmdline2argv(cmdline, &argv);
#endif

	parse_option(argc, argv, &args);

#ifndef CONFIG_IS_CONSOLE_APP
	destroy_dumped_strlist(argv);
#endif

	if (!args.dotsav)
		args.dotsav = get_dotsav_defpath();
	if (!args.dotsav)
		die(_("no configuration file was provided"));
	if (args.output)
		appcon.redirect_stdio(args.output);

	size_t nl = parse_dotsav(args.dotsav, &dotsav);
	if (!nl)
		die(_("no configuration was found in `%s'"), args.dotsav);

	format_dotsav(dotsav, nl);
	DEBUG_RUN()
		print_dotsav(dotsav, nl);

	backup(dotsav);

#ifdef CONFIG_IS_CONSOLE_APP
	exit(0); /* TODO LATER IMPL */
#else
	int err;
	HWND window;

	err = create_app_window(app, &window);
	if (err)
		die(_("unable to initialize main window"));

	NOTIFYICONDATA icon;
	setup_notifyicon(app, window, &icon);
	show_notifyicon(&icon);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	exit(0);
#endif /* CONFIG_IS_CONSOLE_APP */
}

class console *get_app_console()
{
	return &appcon;
}
