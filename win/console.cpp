// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <conio.h>

static FILE *console;

void setup_console()
{
	AllocConsole();

	freopen_s(&console, "CONOUT$", "w", stdout);
	freopen_s(&console, "CONOUT$", "w", stderr);
	freopen_s(&console, "CONIN$", "r", stdin);
}

void waiting_user()
{
	puts("Press any key to continue...");
	_getch();
}
