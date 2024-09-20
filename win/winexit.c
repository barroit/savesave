// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "console.h"
#include "termas.h"

void winexit(int code)
{
#ifndef CONFIG_IS_CONSOLE_APP
	if (code) {
		show_console();

		puts(_("Press any key to continue..."));
		_getch();
	}
#endif

	if (code == 39)
		code = 0;

	exit(code);
}
