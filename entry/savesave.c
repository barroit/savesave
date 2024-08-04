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

static struct cmdarg args;
static struct savesave savconf;

static void handle_option(int argc, char *const *argv)
{
	int err;

	err = parse_option(argc, argv, &args);
	EXIT_ON(err);
}

int main(int argc, char *const *argv)
{
	int err;

	if (argc > 1)
		handle_option(--argc, ++argv);

	err = parse_savesave_config(args.confpath, &savconf);
	EXIT_ON(err);

	return 0;
}
