// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/ui.hpp"
#include "win/resid.hpp"
#include "calc.h"
#include "termsg.h"

static void load_icon_resource(HINSTANCE app, HICON *icon)
{
	int err = LoadIconMetric(app, MAKEINTRESOURCEW(NOTIFY_ICON),
				 LIM_SMALL, icon);
	BUG_ON(err);
}

void setup_notifyicon(HINSTANCE app, HWND window, NOTIFYICONDATA *icon)
{
	memset(icon, 0, sizeof(*icon));
	icon->cbSize = sizeof(*icon);

	icon->hWnd = window;
	icon->uID = 0;

	icon->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;

	icon->uVersion = NOTIFYICON_VERSION_4;
	icon->uCallbackMessage = SS_NOTIFYICON;

	strncpy(icon->szTip, APPNAME " - " SAVESAVE_VERSION,
		ARRAY_SIZEOF(icon->szTip));

	load_icon_resource(app, &icon->hIcon);
}

void show_notifyicon(NOTIFYICONDATA *icon)
{
	int err;

	err = !Shell_NotifyIcon(NIM_ADD, icon);
	BUG_ON(err);

	err = !Shell_NotifyIcon(NIM_SETVERSION, icon);
	BUG_ON(err);
}
