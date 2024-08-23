/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

class console {
	/*
	 * console properties
	 */
	FILE *stream;
	HWND handle;
	bool is_live;

public:
	console() : stream(NULL), handle(NULL), is_live(1) {}

	void setup_console();

	/**
	 * hide_console - hide the console
	 * 
	 * note: by default, the console is not hidden when debugging
	 */
	inline void hide_console()
	{
#ifndef NDEBUG
		return;
#endif
		ShowWindow(handle, SW_HIDE);
	}

	inline void show_console()
	{
		ShowWindow(handle, SW_SHOW);
	}

	void redirect_stdio(const char *output);

	inline bool is_active()
	{
		return is_live;
	}
};

class console *get_console();
