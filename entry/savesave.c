// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "barroit/io.h"
#include "termsg.h"
#include "savconf.h"
#include "debug.h"

static struct cmdarg args;

static struct savesave *savesave_list;
static size_t savesave_nr;

int main(int argc, char *const *argv)
{
	if (argc > 1)
		parse_option(argc, argv, &args);

	if (!args.savconf)
		args.savconf = get_default_savconf_path();
	if (!args.savconf)
		die("no savconf was provided");

	savesave_nr = parse_savconf(args.savconf, &savesave_list);
	DEBUG_RUN(print_savconf, savesave_list, savesave_nr);

	return 0;
}
