// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "termsg.h"
#include "debug.h"

#ifndef CONFIG_DISABLE_CONSOLE_OUTPUT

void console::setup_console()
{
	int err;

	err = !AllocConsole();
	BUG_ON(err);

	stream = freopen("CONOUT$", "w", stdout);
	BUG_ON(!stream);

	stream = freopen("CONOUT$", "w", stderr);
	BUG_ON(!stream);

	err = !SetConsoleOutputCP(CP_UTF8);
	if (err)
		warn("failed to set console code page to utf-8");

	handle = GetConsoleWindow();
	BUG_ON(!handle);
}

void console::redirect_stdio(const char *output)
{
	int err;

	handle = NULL;
	is_live = false;

	/*
	 * dup2() closes stdout and stderr
	 */
	err = ::redirect_stdio(output);
	if (err)
		die("unable to redirect standard io");

	stream = NULL;
	FreeConsole();
}

#endif /* CONFIG_DISABLE_CONSOLE_OUTPUT */
