// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "menu.h"
#include "termas.h"
#include "notifyicon.h"

static int setup_menu(HMENU menu)
{
	MENUINFO info = {
		.cbSize = sizeof(info),
	};

	int err = !SetMenuInfo(menu, &info);
	if (!err)
		return 0;

	return error_winerr(_("failed to set menu infomation"));
}

static int add_menu_button(HMENU menu, uint id, const char *name)
{

	MENUITEMINFO button = {
		.cbSize     = sizeof(button),
		.fMask      = MIIM_STRING | MIIM_ID,
		.fType      = MFT_STRING,
		.wID        = id,
		.dwTypeData = (char *)name,
		.cch        = (UINT)strlen(name),
	};

	int err = !InsertMenuItem(menu, id, 0, &button);
	if (!err)
		return 0;

	return error_winerr("failed to add ‘%s’ button", name);
}

int show_popup_menu(HWND window, POINT *cursor)
{
	int ret;

	HMENU menu = CreatePopupMenu();
	if (!menu)
		return error_winerr("failed to create menu");

	ret = setup_menu(menu);
	if (ret)
		goto cleanup;

	ret = add_menu_button(menu, MNBUT_ABOUT, "About");
	if (ret)
		goto cleanup;

	ret = add_menu_button(menu, MNBUT_CLOSE, "Close");
	if (ret)
		goto cleanup;

	ret = !SetForegroundWindow(window);
	if (ret) {
		error("failed to set window to foreground");
		goto cleanup;
	}

	ret = !TrackPopupMenu(menu,
			      TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
			      cursor->x, cursor->y, 0, window, NULL);
	if (ret)
		error_winerr("failed to track popup menu");

cleanup:
	DestroyMenu(menu);
	return ret;
}
