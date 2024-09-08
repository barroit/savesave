// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "win/ui.hpp"
#include "win/termas.hpp"
#include "robio.h"
#include "win/cmdline.hpp"
#include "win/backup.hpp"
#include "savconf.h"
#include "debug.h"
#include "win/atenter.hpp"
#include "win/savconf.hpp"

static class console appcon;
static struct cmdarg args;
static struct savesave *savconf;

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	setup_message_i18n();

	int err;
	/*
	 * class constructor is guaranteed to run before main function, use
	 * that for constructor function leak support on windows
	 */
	static class atenter constructor;

	appcon.setup_console();
	constructor.precheck();

	parse_cmdline(cmdline, &args);
	if (args.output)
		appcon.redirect_stdio(args.output);

	size_t confcnt = parse_savconf(args.savconf, &savconf);
	if (!confcnt)
		die(_("`%s' must have at least one configuration"),
		    args.savconf);
	format_savconf(savconf, confcnt);

	DEBUG_RUN()
		print_savconf(savconf, confcnt);

	// backup bu{ nconf };
	// bu.create_backup_task(savconf);

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
}

class console *get_app_console()
{
	return &appcon;
}
