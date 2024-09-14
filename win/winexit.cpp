// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "termas.h"
#include "win/console.hpp"

static void waiting_user()
{
	puts(_("Press any key to continue..."));
	_getch();
}

void winexit(int code)
{
	if (code && !IS_DEFINED(CONFIG_IS_CONSOLE_APP)) {
		class console *con = get_app_console();
		if (con->is_active())
			con->show_console();
		waiting_user();
	}

	exit(code);
}
