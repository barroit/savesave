// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

void setup_console();

int cmdline2argv(const char *cmdline, char ***argv);

void rmargv(int argc, char **argv);

void waiting_user();
