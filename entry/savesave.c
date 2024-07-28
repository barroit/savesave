// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "barroit/io.h"
#include "termsg.h"
#include "getconf.h"

static const char *savconf_path;
static struct savesave savconf;

static void handle_option(int argc, char *const *argv)
{
	enum optid res = parse_option(argc, argv);

	if (res == OPT_VERSION || res == OPT_HELP)
		exit(128);
	else if (res == OPT_CONFIG)
		savconf_path = optarg;
}

int main(int argc, char *const *argv)
{
	int err;

	if (argc > 1)
		handle_option(argc, argv);

	err = parse_savesave_config(savconf_path, &savconf);
	EXIT_ON(err);

	return 0;
}
