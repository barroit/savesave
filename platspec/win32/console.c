// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "console.h"
#include "termas.h"

static void adjust_console_size(short size)
{
	int err;
	HANDLE console = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
				    0, NULL, OPEN_EXISTING,
				    FILE_ATTRIBUTE_NORMAL, NULL);

	if (console == INVALID_HANDLE_VALUE)
		return;

	CONSOLE_SCREEN_BUFFER_INFO info;
	err = !GetConsoleScreenBufferInfo(console, &info);
	if (err)
		goto err_stat_console;
	else if (info.dwSize.X >= size)
		goto cleanup;

	COORD coord = {
		.X = size,
		.Y = info.dwSize.Y,
	};

	err = !SetConsoleScreenBufferSize(console, coord);
	if (err)
		goto err_adjust_console;

	SMALL_RECT rect = {
		.Left   = 0,
		.Top    = 0,
		.Right  = (short)(coord.X - 1),
		.Bottom = 24,
	};

	err = !SetConsoleWindowInfo(console, TRUE, &rect);
	if (err)
		goto err_update_console;
	
	if (0) {
	err_stat_console:
		warn_winerr(_("failed to get console screen information"));
	} else if (0) {
	err_adjust_console:
		warn_winerr(_("failed to set console screen size"));
	} else if (0) {
	err_update_console:
		warn_winerr(_("failed to update console window"));
		SetConsoleScreenBufferSize(console, info.dwSize);
	}

cleanup:
	CloseHandle(console);
}

static void setup_console_codepage(void)
{
	int err = !SetConsoleOutputCP(CP_UTF8);
	if (!err)
		return;

	warn_winerr(_("failed to set console output codepage to UTF-8"));
}

void setup_console(void)
{
	int err;
	FILE *stream;

	err = !AllocConsole();
	BUG_ON(err);

	stream = freopen("CONOUT$", "w", stdout);
	BUG_ON(!stream);

	stream = freopen("CONOUT$", "w", stderr);
	BUG_ON(!stream);

	HWND console = GetConsoleWindow();
	BUG_ON(!console);

	adjust_console_size(CONFIG_LINEWRAP_THRESHOLD);

	ShowWindow(console, SW_HIDE);

	setup_console_codepage();
}

void show_console(void)
{
	HWND console = GetConsoleWindow();
	BUG_ON(!console);

	ShowWindow(console, SW_SHOW);
}
