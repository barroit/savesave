// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "win/termas.hpp"
#include "win/console.hpp"

static void waiting_user()
{
	puts("Press any key to continue...");
	_getch();
}

void winexit(int code)
{
	if (code) {
		class console *con = get_app_console();
		if (con->is_active())
			con->show_console();
		waiting_user();
	}

	exit(code);
}
