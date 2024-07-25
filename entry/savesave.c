// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include <stdlib.h>
#include "barroit/io.h"
#include "termsg.h"
#include "getconf.h"

static const char *config_path;
static struct savesave savconf;

static void handle_option(int argc, char *const *argv)
{
	enum optid res = parse_option(argc, argv);

	if (res == OPT_VERSION || res == OPT_HELP)
		exit(0);
	else if (res == OPT_CONFIG)
		config_path = optarg;
}

#include <stdio.h>

int main(int argc, char *const *argv)
{
	if (argc > 1)
		handle_option(argc, argv);

	char *strconf = read_config(config_path);
	if (!strconf)
		exit(128);

	int err = parse_config(strconf, &savconf);
	if (err)
		exit(128);

	free(strconf);
	printf("save: %s\nbackup: %s\ninterval: %u\nstack: %u\n",
	       savconf.save, savconf.backup, savconf.interval, savconf.stack);

	return 0;
}
