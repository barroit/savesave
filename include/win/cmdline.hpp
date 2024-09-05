/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#include "runopt.h"
#include "savconf.h"

void parse_cmdline(const char *cmdline, struct cmdarg *args);
