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
	BUG_ON(code == STILL_ACTIVE);

#ifndef CONFIG_IS_CONSOLE_APP
	if (code) {
		show_console();

		puts(_("Press any key to continue..."));
		_getch();
	}
#endif

	/*
	 * we need a magic value to make a console display even if there's no
	 * error, so miku is used
	 */
	if (code == 39)
		code = 0;

	exit(code);
}
