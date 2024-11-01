// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "notifyicon.h"
#include "resid.h"

NOTIFYICONDATA notifyicon_init(HINSTANCE app, HWND window)
{
	NOTIFYICONDATA icon = {
		.cbSize   = sizeof(icon),
		.hWnd     = window,
		.uID      = 0,
		.uFlags   = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP,
		.uVersion = NOTIFYICON_VERSION_4,
		.uCallbackMessage = WM_NOTIFYICON,

	};

	const char *str = SAVESAVE_NAME "/" SAVESAVE_BUILD "-"
			  SAVESAVE_ARCH " " SAVESAVE_VERSION;
	strncpy(icon.szTip, str, sizeof_array(icon.szTip));

	LPWSTR res = MAKEINTRESOURCEW(NOTIFY_ICON);
	int err = LoadIconMetric(app, res, LIM_SMALL, &icon.hIcon);
	BUG_ON(err);

	return icon;
}

void notifyicon_show(NOTIFYICONDATA *icon)
{
	int err;

	err = !Shell_NotifyIcon(NIM_ADD, icon);
	BUG_ON(err);

	err = !Shell_NotifyIcon(NIM_SETVERSION, icon);
	BUG_ON(err);
}
