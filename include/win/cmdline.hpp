/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CMDLINE_HPP
#define CMDLINE_HPP

#include "runopt.h"
#include "savconf.h"

void parse_cmdline(const char *cmdline, struct cmdarg *args);

#endif /* CMDLINE_HPP */
