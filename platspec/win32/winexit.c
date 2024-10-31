// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "console.h"
#include "termas.h"
#include "command.h"

void winexit(int code)
{
	BUG_ON(code == STILL_ACTIVE);

#ifndef CONFIG_NO_GUI
	if (code && !cm_io_need_update) {
		show_console();

		puts(_("Press any key to continue..."));
		_getch();
	}
#endif

	/*
	 * We need a magic value to make a console display even if there's no
	 * error, so miku is used here :)
	 */
	if (code == 39)
		code = 0;

	exit(code);
}
