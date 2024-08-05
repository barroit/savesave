// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/ui.hpp"
#include "win/termsg.hpp"

#define CLASS_ID APPNAME

static bool is_request_menu(unsigned type)
{
	return type == WM_CONTEXTMENU ||
	       type == NIN_KEYSELECT ||
	       type == NIN_SELECT;
}

static LRESULT handle_notifyicon(HWND window, UINT message,
				 WPARAM arg1, LPARAM arg2)
{
	unsigned type = LOWORD(arg2);
	if (is_request_menu(type)) {
		POINT cursor = {
			.x = GET_X_LPARAM(arg1),
			.y = GET_Y_LPARAM(arg1),
		};

		return show_popup_menu(window, &cursor);
	}

	return DefWindowProc(window, message, arg1, arg2);
}

static LRESULT CALLBACK receive_window_message(HWND window, UINT message,
					       WPARAM arg1, LPARAM arg2)
{
	switch (message) {
	case SS_NOTIFYICON:
		return handle_notifyicon(window, message, arg1, arg2);
	case WM_MENUCOMMAND:
		return S_OK;
	case WM_DESTROY:
		PostQuitMessage(0);
		return S_OK;
	default:
		return DefWindowProc(window, message, arg1, arg2);
	}
}

int create_app_window(HINSTANCE app, HWND *window)
{
	WNDCLASSEX wclass = {
		.cbSize        = sizeof(WNDCLASSEX),
		.lpfnWndProc   = receive_window_message,
		.hInstance     = app,
		.lpszClassName = CLASS_ID,
	};

	ATOM id = RegisterClassEx(&wclass);
	if (!id)
		return error_winerr("failed to register window class");

	*window = CreateWindowEx(0, CLASS_ID, NULL, WS_MINIMIZE, CW_USEDEFAULT,
				 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				 NULL, NULL, app, NULL);
	if (!(*window))
		return error_winerr("unable to create window");

	return 0;
}
