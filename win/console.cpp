// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/console.hpp"
#include "win/autovar.hpp"
#include "termas.h"
#include "debug.h"
#include "ioop.h"
#include "poison.h"

static HANDLE create_conhand()
{
	return CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, 0, NULL,
			  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static void release_conhand(HANDLE *con)
{
	CloseHandle(*con);
}

static void set_console_size(short size)
{
	class autovar<HANDLE> con{ create_conhand, release_conhand };
	int err;

	CONSOLE_SCREEN_BUFFER_INFO info;
	err = !GetConsoleScreenBufferInfo(*con, &info);
	if (err)
		return;
	if (info.dwSize.X >= size)
		return;

	COORD coord = {
		.X = size,
		.Y = info.dwSize.Y,
	};

	err = !SetConsoleScreenBufferSize(*con, coord);
	if (err)
		return;

	SMALL_RECT rect = {
		.Left   = 0,
		.Top    = 0,
		.Right  = (short)(coord.X - 1),
		.Bottom = 24,
	};

	err = !SetConsoleWindowInfo(*con, TRUE, &rect);
	if (err)
		SetConsoleScreenBufferSize(*con, info.dwSize);
}

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
		warn(_("failed to set console code page to utf-8"));

	window = GetConsoleWindow();
	BUG_ON(!window);

	set_console_size(CONFIG_LINEWRAP_THRESHOLD);
	hide_console();
}

void console::redirect_stdio(const char *output)
{
	int err;

	window = (HWND)GENERIC_POISON;
	is_live = false;

	/*
	 * dup2() closes stdout and stderr
	 */
	err = ::redirect_stdio(output);
	if (err)
		die(_("unable to redirect standard io"));

	stream = (FILE *)GENERIC_POISON;
	FreeConsole();
}

#endif /* CONFIG_DISABLE_CONSOLE_OUTPUT */
