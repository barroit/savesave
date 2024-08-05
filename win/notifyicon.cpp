// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/notifyicon.hpp"
#include "calc.h"
#include "win/resid.hpp"
#include "termsg.h"

void init_notifyicon(HWND window, NOTIFYICONDATA *icon)
{
	memset(icon, 0, sizeof(*icon));

	icon->cbSize = sizeof(*icon);

	icon->hWnd = window;
	icon->uID = 0;

	icon->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;

	icon->uVersion = NOTIFYICON_VERSION_4;
	icon->uCallbackMessage = NOTIFYICON_CLICK;

	strncpy(icon->szTip, APPNAME "-" SAVESAVE_VERSION,
		ARRAY_SIZEOF(icon->szTip));
}

int load_icon_resource(HINSTANCE app, NOTIFYICONDATA *icon)
{
	int err;

	err = LoadIconMetric(app, MAKEINTRESOURCEW(NOTIFY_ICON),
			     LIM_SMALL, &icon->hIcon);
	if (err)
		return error("failed to load icon resource");

	return 0;
}

int show_icon(NOTIFYICONDATA *icon)
{
	int err;

	err = !Shell_NotifyIcon(NIM_ADD, icon);
	if (err)
		return error("failed to add the notification icon");

	return 0;
}
