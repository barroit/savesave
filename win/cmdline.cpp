// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/cmdline.hpp"
#include "alloc.h"
#include "list.h"
#include "termsg.h"

void uarg_parser::dump_cmdline(const char *cmdline)
{
	std::string line = std::string(APPNAME) + " " + std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string val;

	while (stream >> std::quoted(val)) {
		CAP_ALLOC(&argv, argc + 1, &cap);
		argv[argc++] = xstrdup(val.c_str());
	}

	CAP_ALLOC(&argv, argc + 1, &cap);
	argv[argc] = NULL;
}

void uarg_parser::parse_cmdline()
{
	int err;

	err = parse_option(argc, argv, &args);
	EXIT_ON(err);
}

void uarg_parser::parse_savconf()
{
	int err;

	err = parse_savesave_config(args.config, &savconf);
	EXIT_ON(err);
}
