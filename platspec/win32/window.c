// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "window.h"
#include "termas.h"
#include "notifyicon.h"
#include "menu.h"

static LRESULT CALLBACK receive_message(HWND window, UINT message,
					WPARAM arg1, LPARAM arg2)
{
	switch (message) {
	case WM_NOTIFYICON:
		uint type = LOWORD(arg2);

		if (type != WM_CONTEXTMENU &&
		    type != NIN_SELECT &&
		    type != NIN_KEYSELECT)
			break;

		POINT cursor = {
			.x = GET_X_LPARAM(arg1),
			.y = GET_Y_LPARAM(arg1),
		};

		return show_popup_menu(window, &cursor);
	case WM_COMMAND:
		if (HIWORD(arg1) != 0)
			break;

		switch (LOWORD(arg1)) {
		case MNBUT_CLOSE:
			DestroyWindow(window);
			break;
		case MNBUT_ABOUT:
			warn(_("'About' feature is not available yet"));
		}

		return S_OK;
	case WM_DESTROY:
		PostQuitMessage(0);
		return S_OK;
	}

	return DefWindowProc(window, message, arg1, arg2);
}

HWND create_main_window(HINSTANCE app)
{
	WNDCLASSEX class = {
		.cbSize        = sizeof(class),
		.lpfnWndProc   = receive_message,
		.hInstance     = app,
		.lpszClassName = SAVESAVE_NAME,
	};

	ATOM id = RegisterClassEx(&class);
	if (!id)
		die_winerr(_("failed to register window class"));

	HWND window = CreateWindowEx(0, SAVESAVE_NAME, NULL, WS_MINIMIZE,
				     CW_USEDEFAULT, CW_USEDEFAULT,
				     CW_USEDEFAULT, CW_USEDEFAULT,
				     NULL, NULL, app, NULL);
	if (!window)
		die_winerr(_("failed to create window"));

	return window;
}
