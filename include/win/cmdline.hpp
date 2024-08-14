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
	char **argv;
	size_t argc;

public:
	struct cmdarg args;
	struct savesave *savconf;

	uarg_parser() : argv(NULL), argc(0), savconf(NULL)
	{
		args = { 0 };
	}

	void dump_cmdline(const char *cmdline);

	void parse_cmdline();

	void parse_savconf();
};
