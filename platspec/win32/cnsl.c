// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "cnsl.h"
#include "termas.h"

static HWND cnsl;

static void cnsl_window_size(short size)
{
	int err;
	HANDLE con = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
				0, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL);
	BUG_ON(con == INVALID_HANDLE_VALUE);

	CONSOLE_SCREEN_BUFFER_INFO info;
	err = !GetConsoleScreenBufferInfo(con, &info);
	if (err) {
		warn_winerr(_("failed to get console screen information"));
		goto cleanup;
	} else if (info.dwSize.X >= size) {
		goto cleanup;
	}

	COORD coord = {
		.X = size,
		.Y = info.dwSize.Y,
	};

	err = !SetConsoleScreenBufferSize(con, coord);
	if (err) {
		warn_winerr(_("failed to set console screen size"));
		goto cleanup;
	}

	SMALL_RECT rect = {
		.Left   = 0,
		.Top    = 0,
		.Right  = (short)(coord.X - 1),
		.Bottom = 24,
	};

	err = !SetConsoleWindowInfo(con, TRUE, &rect);
	if (err) {
		warn_winerr(_("failed to update console window"));
		SetConsoleScreenBufferSize(con, info.dwSize);
		goto cleanup;
	}

cleanup:
	CloseHandle(con);
}

#define xfreopen(pathname, mode, stream)				\
do {									\
	FILE *__xfreopen_stream = freopen(pathname, mode, stream);	\
	BUG_ON(!__xfreopen_stream);					\
} while (0)

void cnsl_attach(void)
{
	/*
	 * DO NOT do this, or you will get a SUPER DUMB TERMINAL.
	 * AttachConsole(ATTACH_PARENT_PROCESS);
	 */
	int err = AllocConsole();
	BUG_ON(err);

	xfreopen("CONIN$", "r", stdin);
	xfreopen("CONOUT$", "w", stdout);
	xfreopen("CONOUT$", "w", stderr);

	cnsl_window_size(CONFIG_LINEWRAP_THRESHOLD);

	cnsl = GetConsoleWindow();
	cnsl_hide();

	err = !SetConsoleOutputCP(CP_UTF8);
	if (err)
		warn_winerr(_("failed to set console output codepage to UTF-8"));
}

void cnsl_show(void)
{
	ShowWindow(cnsl, SW_SHOW);
}

void cnsl_hide(void)
{
	ShowWindow(cnsl, SW_HIDE);
}
