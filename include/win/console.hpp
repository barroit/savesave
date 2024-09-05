/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#ifdef CONFIG_DISABLE_CONSOLE_OUTPUT

#define _getch() do {} while (0)

class console {
	inline void setup_console() {}

	inline void hide_console() {}
	inline void show_console() {}

	inline void redirect_stdio(const char *output) {}

	inline bool is_active() { return false; }
}

#else

class console {
	FILE *stream;
	HWND handle;
	bool is_live;

public:
	console() : stream(NULL), handle(NULL), is_live(1) {}

	void setup_console();

	inline void hide_console()
	{
# ifndef NDEBUG
		ShowWindow(handle, SW_HIDE);
# endif
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

#endif /* CONFIG_DISABLE_CONSOLE_OUTPUT */

class console *get_app_console();
