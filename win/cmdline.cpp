// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/cmdline.hpp"
#include "termsg.h"
#include "strlist.h"
#include "list.h"
#include "strbuf.h"

void uarg_parser::dump_cmdline(const char *cmdline)
{
	std::string line = std::string(APPNAME) + " " + std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string val;
	struct strlist sl = STRLIST_INIT_DUPE;

	while (stream >> std::quoted(val))
		strlist_push(&sl, val.c_str());
	strlist_terminate(&sl);

	argv = sl.list;
	argc = sl.nl;
}

void uarg_parser::parse_cmdline()
{
	parse_option(argc, argv, &args);

	if (!args.savconf)
		args.savconf = get_default_savconf_path();
	if (!args.savconf)
		die("no savconf was provided");
}

void uarg_parser::parse_savconf()
{
	nconf = ::parse_savconf(args.savconf, &savconf);

	size_t i;
	for_each_idx(i, nconf) {
		struct savesave *c = &savconf[i];
		str_replace(c->save, '\\', '/');
		str_replace(c->backup, '\\', '/');
	}
}
