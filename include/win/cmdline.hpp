/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#include "runopt.h"
#include "savconf.h"

class uarg_parser {
public:
	struct cmdarg args;
	struct savesave *savconf;
	size_t nconf;

	uarg_parser() : savconf(NULL), nconf(0)
	{
		args = { 0 };
	}

	void parse_cmdline(const char *cmdline);

	void parse_savconf();
};
