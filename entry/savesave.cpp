// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "windows.h"
#include "win/cmdline.hpp"

int WINAPI WinMain(HINSTANCE app, HINSTANCE, char *cmdline, int)
{
	if (*cmdline != 0)
		return parse_cmdline(cmdline);

	return 0;
}
