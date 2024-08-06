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

static int setup_menu(HMENU menu)
{
	int err;

	MENUINFO info = {
		.cbSize  = sizeof(MENUINFO),
	};

	err = !SetMenuInfo(menu, &info);
	if (err)
		return error_winerr("failed to set menu infomation");

	return 0;
}

static int add_menu_button(HMENU menu, unsigned id, const char *name)
{
	int err;

	MENUITEMINFO button = {
		.cbSize     = sizeof(MENUITEMINFO),
		.fMask      = MIIM_STRING | MIIM_ID,
		.fType      = MFT_STRING,
		.wID        = id,
		.dwTypeData = (char *)name,
		.cch        = (UINT)strlen(name),
	};

	err = !InsertMenuItem(menu, id, false, &button);
	if (err)
		return error_winerr("failed to add ‘%s’ button", name);

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

	err = add_menu_button(menu, BUTTON_ABOUT, "About");
	if (err)
		goto cleanup;

	err = add_menu_button(menu, BUTTON_CLOSE, "Close");
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
