/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#ifdef CONFIG_DISABLE_CONSOLE_OUTPUT

#define _getch() do {} while (0)

class console {
public:
	inline void setup_console() {}

	inline void hide_console() {}
	inline void show_console() {}

	inline void redirect_stdio(const char *output) {}

	inline bool is_active() { return false; }
};

#else /* NOT CONFIG_DISABLE_CONSOLE_OUTPUT */

class console {
	FILE *stream;
	HWND window;
	bool is_live;

public:
	console() : stream(NULL), window(NULL), is_live(1) {}

	void setup_console();

	inline void hide_console()
	{
# ifndef NDEBUG
		ShowWindow(window, SW_HIDE);
# endif
	}

	inline void show_console()
	{
		ShowWindow(window, SW_SHOW);
	}

	void redirect_stdio(const char *output);

	inline bool is_active()
	{
		return is_live;
	}
};

#endif /* CONFIG_DISABLE_CONSOLE_OUTPUT */

void setup_console_codepage();

class console *get_app_console();

#endif /* CONSOLE_HPP */
