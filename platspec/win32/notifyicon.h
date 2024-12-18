/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NOTIFYICON_H
#define NOTIFYICON_H

#define WM_NOTIFYICON (WM_APP + 1)

NOTIFYICONDATA notifyicon_init(HINSTANCE app, HWND window);

void notifyicon_show(NOTIFYICONDATA *icon);

#endif /* NOTIFYICON_H */
