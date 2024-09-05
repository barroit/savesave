// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/ui.hpp"
#include "win/termas.hpp"

#define CLASS_ID APPNAME

static bool is_request_menu(LPARAM arg)
{
	unsigned type = LOWORD(arg);
	return type == WM_CONTEXTMENU ||
	       type == NIN_KEYSELECT ||
	       type == NIN_SELECT;
}

static LRESULT handle_notifyicon(HWND window, LONG x, LONG y)
{
	POINT cursor = {
		.x = x,
		.y = y,
	};

	return show_popup_menu(window, &cursor);
}

static int is_menu_command(WPARAM arg)
{
	return HIWORD(arg) == 0;
}

static LRESULT handle_menu_click(HWND window, enum menu_button button)
{
	switch (button) {
	case BUTTON_CLOSE:
		DestroyWindow(window);
		break;
	case BUTTON_ABOUT:
		warn("‘About’ feature is not available yet");
	}

	return 0;
}

static LRESULT CALLBACK receive_window_message(HWND window, UINT message,
					       WPARAM arg1, LPARAM arg2)
{
	switch (message) {
	case SS_NOTIFYICON:
		if (!is_request_menu(arg2))
			break;
		return handle_notifyicon(window, GET_X_LPARAM(arg1),
					 GET_Y_LPARAM(arg1));
	case WM_COMMAND:
		if (!is_menu_command(arg1))
			break;
		return handle_menu_click(window,
					 (enum menu_button)LOWORD(arg1));
	case WM_DESTROY:
		PostQuitMessage(0);
		return S_OK;
	}

	return DefWindowProc(window, message, arg1, arg2);
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
		return error_winerr("failed to create window");

	return 0;
}
