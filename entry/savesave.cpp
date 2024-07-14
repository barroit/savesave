// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "windows.h"
#include "win/cmdline.hpp"

#ifdef UNICODE
#define REALMAIN wWinMain
#else
#define REALMAIN WinMain
#endif

int WINAPI REALMAIN(HINSTANCE app, HINSTANCE, TCHAR *cmdline, int)
{
	if (*cmdline != 0)
		return parse_cmdline();

	return 0;
}
