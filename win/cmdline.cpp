// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/cmdline.hpp"
#include "termas.h"
#include "strlist.h"
#include "list.h"
#include "strbuf.h"
#include "keepref.h"

static void dump_cmdline(const char *cmdline, int *argc, char ***argv)
{
	std::string line = std::string(APPNAME) + " " + std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string opt;
	struct strlist sl;

	strlist_init(&sl, STRLIST_DUPSTR);
	while (stream >> std::quoted(opt))
		strlist_push(&sl, opt.c_str());

	*argc = (int)sl.nl;
	*argv = strlist_dump(&sl);
	strlist_destroy(&sl);
}

void parse_cmdline(const char *cmdline, struct cmdarg *args)
{
	char **argv;
	int argc;
	dump_cmdline(cmdline, &argc, &argv);

	parse_option(argc, argv, args);
	destroy_dumped_strlist(argv);

	if (!args->savconf)
		args->savconf = get_default_savconf_path();
	if (!args->savconf)
		die("no savconf was provided");
}
