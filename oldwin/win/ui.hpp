/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef UI_HPP
#define UI_HPP

int create_app_window(HINSTANCE app, HWND *window);

enum menu_button {
	BUTTON_CLOSE,
	BUTTON_ABOUT,
};

int show_popup_menu(HWND window, POINT *cursor);

#define SS_NOTIFYICON (WM_APP + 1)

void setup_notifyicon(HINSTANCE app, HWND window, NOTIFYICONDATA *icon);

void show_notifyicon(NOTIFYICONDATA *icon);

#endif /* UI_HPP */
