// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "robio.h"
#include "termas.h"
#include "savconf.h"
#include "debug.h"
#include "backup.h"

static struct cmdarg args;

static struct savesave *savesave_list;
static size_t savesave_nr;

int main(int argc, char *const *argv)
{
	setup_message_i18n();

	if (argc > 1)
		parse_option(argc, argv, &args);

	if (!args.savconf)
		args.savconf = get_default_savconf_path();
	if (!args.savconf)
		die("no savconf was provided");

	savesave_nr = parse_savconf(args.savconf, &savesave_list);
	if (!savesave_nr)
		die("‘%s’ must contain at least one configuration",
		    args.savconf);
	DEBUG_RUN()
		print_savconf(savesave_list, savesave_nr);

	backup_routine(savesave_list);

	exit(0);
}
