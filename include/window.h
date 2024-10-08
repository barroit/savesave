/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef _WIN32
# error "window.h is win-spec"
#endif

#ifndef WINDOW_H
#define WINDOW_H

HWND create_main_window(HINSTANCE app);

#endif /* WINDOW_H */
