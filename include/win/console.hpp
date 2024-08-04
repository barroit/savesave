// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

int cmdline2argv(const char *cmdline, char ***argv);

void setup_console();

void hide_console();

void show_console();

void teardown_console();

void waiting_user();
