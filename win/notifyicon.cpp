// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/notifyicon.hpp"
#include "calc.h"

void init_notifyicon_data(HWND window, NOTIFYICONDATA *data)
{
	memset(data, 0, sizeof(*data));

	data->cbSize = sizeof(*data);

	data->hWnd = window;
	data->uID = 0;

	data->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;

	data->uVersion = NOTIFYICON_VERSION_4;
	data->uCallbackMessage = NOTIFYICON_CLICK;

	strncpy(data->szTip, APPNAME "-" SAVESAVE_VERSION,
		ARRAY_SIZEOF(data->szTip));
}
