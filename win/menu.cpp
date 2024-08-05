// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/ui.hpp"
#include "win/termsg.hpp"
#include "barroit/types.h"
#include "alloc.h"

#define ARGB_MASK 0xFF

static int setup_menu(HMENU menu)
{
	int err;

	MENUINFO info = {
		.cbSize  = sizeof(MENUINFO),
		.fMask   = MIM_STYLE,
		.dwStyle = MNS_NOTIFYBYPOS,
	};

	err = !SetMenuInfo(menu, &info);
	if (err)
		return error_winerr("failed to set menu infomation");

	return 0;
}

static int populate_menu_item(HMENU menu)
{
	int err;

	MENUITEMINFO button = {
		.cbSize = sizeof(MENUITEMINFO),
		.fMask  = MIIM_FTYPE,
		.fType  = MFT_OWNERDRAW,
	};

	err = !InsertMenuItem(menu, BUTTON_CLOSE, false, &button);
	if (err)
		return error_winerr("failed to add ‘Close’ button");

	err = !InsertMenuItem(menu, BUTTON_CLOSE, false, &button);
	if (err)
		return error_winerr("failed to add ‘About’ button");

	return 0;
}

int show_popup_menu(HWND window, POINT *cursor)
{
	int err;

	HMENU menu = CreatePopupMenu();
	if (!menu)
		return error_winerr("failed to create menu");

	err = setup_menu(menu);
	if (err)
		goto cleanup;

	err = populate_menu_item(menu);
	if (err)
		goto cleanup;

	err = !SetForegroundWindow(window);
	if (err) {
		error("failed to set window to foreground");
		goto cleanup;
	}

	err = !TrackPopupMenu(menu,
			      TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
			      cursor->x, cursor->y, 0, window, NULL);
	if (err)
		error_winerr("failed to track popup menu");

cleanup:
	DestroyMenu(menu);

	return !!err;
}
