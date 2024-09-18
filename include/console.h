/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef _WIN32
# error "console.h is win-spec"
#endif

#ifndef CONSOLE_H
#define CONSOLE_H

void setup_console(void);

void show_console(void);

#endif /* CONSOLE_H */
