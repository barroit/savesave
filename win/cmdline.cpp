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
#include "keepref.h"

static void dump_cmdline(const char *cmdline, size_t *argc, char ***argv)
{
	std::string line = std::string(APPNAME) + " " + std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string opt;
	struct strlist sl;

	strlist_init(&sl, STRLIST_DUPSTR);
	while (stream >> std::quoted(opt))
		strlist_push(&sl, opt.c_str());

	*argc = sl.nl;
	*argv = strlist_dump(&sl);
	strlist_destroy(&sl);
}

void uarg_parser::parse_cmdline(const char *cmdline)
{
	char **argv;
	size_t argc;
	dump_cmdline(cmdline, &argc, &argv);

	parse_option(argc, argv, &args);
	destroy_dumped_strlist(argv);

	if (!args.savconf)
		args.savconf = get_default_savconf_path();
	if (!args.savconf)
		die("no savconf was provided");
	
	KEEPREF(args);
}

void uarg_parser::parse_savconf()
{
	nconf = ::parse_savconf(args.savconf, &savconf);

	size_t i;
	for_each_idx(i, nconf) {
		struct savesave *c = &savconf[i];
		strrepl(c->save_prefix, '\\', '/');
		strrepl(c->backup_prefix, '\\', '/');
	}
}
