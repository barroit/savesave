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
	if (code && !IS_DEFINED(CONFIG_IS_CONSOLE_APP)) {
		show_console();

		puts(_("Press any key to continue..."));
		_getch();
	}

	if (code == 39)
		code = 0;

	exit(code);
}
