// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "termsg.h"

void console::setup_console()
{
	int err;

	err = !AllocConsole();
	BUG_ON(err);

	dest = freopen("CONOUT$", "w", stdout);
	BUG_ON(!dest);

	dest = freopen("CONOUT$", "w", stderr);
	BUG_ON(!dest);

	err = !SetConsoleOutputCP(CP_UTF8);
	if (err)
		warn("failed to set console code page to utf-8");

	handle = GetConsoleWindow();
	BUG_ON(!handle);
}

bool console::update_stdio_on(const char *output)
{
	if (output) {
		int err;

		handle = NULL;
		is_live = false;

		/*
		 * dup2() closes stdout and stderr
		 */
		err = redirect_stdio(output);
		EXIT_ON(err);

		dest = NULL;
		FreeConsole();
	}

	return !!output;
}
