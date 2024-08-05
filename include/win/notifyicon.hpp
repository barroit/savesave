/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#define NOTIFYICON_CLICK (WM_APP + 1)

void init_notifyicon(HWND window, NOTIFYICONDATA *icon);

int load_icon_resource(HINSTANCE app, NOTIFYICONDATA *icon);

int show_icon(NOTIFYICONDATA *icon);
