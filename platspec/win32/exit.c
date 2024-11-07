// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "cnsl.h"
#include "termas.h"
#include "command.h"

#ifdef exit
# undef exit
#endif

void __exit(int code)
{
	BUG_ON(code == STILL_ACTIVE);

#ifndef CONFIG_NO_WIN_GUI
	/*
	 * Run this only if
	 *	process is not running on PROC_DO_MINIMAL.
	 *	output is not redirected to a new file.
	 */
	if (code && !cm_do_mr && !cm_io_need_update) {
		cnsl_show();

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
