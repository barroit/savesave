/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef MENU_H
#define MENU_H

enum menubtn {
	MNBUT_CLOSE,
	MNBUT_ABOUT,
};

int show_popup_menu(HWND window, POINT *cursor);

#endif /* MENU_H */
