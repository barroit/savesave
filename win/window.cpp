// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/window.hpp"
#include "win/notifyicon.hpp"
#include "win/termsg.hpp"

#define CLASS_ID APPNAME

static LRESULT CALLBACK handle_window_message(HWND window, UINT message,
					      WPARAM arg1, LPARAM arg2)
{
	switch (message) {
	case NOTIFYICON_CLICK:
		return S_OK;
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
		.lpfnWndProc   = handle_window_message,
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
