/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

class console {
	FILE *dest;
	HWND handle;
	bool is_live;

public:
	console() : dest(NULL), handle(NULL), is_live(1) {}

	void setup_console();

	inline void hide_console()
	{
		ShowWindow(handle, SW_HIDE);
	}

	inline void show_console()
	{
		ShowWindow(handle, SW_SHOW);
	}

	bool update_stdio_on(const char *output);

	inline bool is_active()
	{
		return is_live;
	}
};

class console *get_console();
