// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#undef exit

#include "win/console.hpp"

void winexit(int code)
{
	if (code)
		waiting_user();

	exit(code);
}
