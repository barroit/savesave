/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#define NOTIFYICON_CLICK (WM_APP + 1)

void init_notifyicon_data(HWND hwnd, NOTIFYICONDATA *data);

