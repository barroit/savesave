// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "win/termsg.hpp"

bool is_console_output();

void winexit(int code)
{
	if (code) {
		if (is_console_output())
			show_console();
		waiting_user();
	}

	exit(code);
}
