// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"

static FILE *output;

void setup_console()
{
	AllocConsole();
	SetConsoleOutputCP(CP_UTF8);

	output = freopen("CONOUT$", "w", stdout);
	BUG_ON(!output);

	output = freopen("CONOUT$", "w", stderr);
	BUG_ON(!output);
}

void hide_console()
{
	HWND console = GetConsoleWindow();
	BUG_ON(!console);

	ShowWindow(console, SW_HIDE);
}

void show_console()
{
	HWND console = GetConsoleWindow();
	BUG_ON(!console);

	ShowWindow(console, SW_SHOW);
}

void teardown_console()
{
	fclose(output);

	FreeConsole();
}

void waiting_user()
{
	puts("Press any key to continue...");
	_getch();
}
