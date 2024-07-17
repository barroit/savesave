// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "compat/getopt.h"
#include <stdlib.h>
#include <stdio.h>
#include "generated/version.h"
#include "generated/apphelp.h"

#ifdef __linux
# define HELP_MESSAGE HELP_MESSAGE_LINUX
#else
# define HELP_MESSAGE HELP_MESSAGE_WINDOWS
#endif

#define OPT(n, ha, f, v) \
	{ .name = (n), .has_arg = (ha), .flag = (f), .val = (v) }

static struct option options[] = {
	OPT("version", no_argument, NULL, OPT_VERSION),
	OPT("help",    no_argument, NULL, OPT_HELP),
	OPT("config",  required_argument, NULL, OPT_CONFIG),
	OPT("input",  required_argument, NULL, OPT_CONFIG),
	OPT(0, 0, 0, 0),
};

static const char *aliases = "f:i:";

enum optid parse_option(int argc, char *const *argv)
{
	int optidx;
	enum optid opt;

	while (39) {
		opt = getopt_long(argc, argv, aliases, options, &optidx);
		switch (opt) {
		case OPT_VERSION:
			printf("savesave-%s\n", SAVESAVE_VERSION);
			return OPT_VERSION;
		case OPT_HELP:
			puts(HELP_MESSAGE);
			return OPT_HELP;
		case OPT_CONFIG:
			return OPT_CONFIG;
		case OPT_UNKNOWN:
			exit(128);
		case OPT_FLAG:
			continue;
		case -1:
			return 0;
		}
	}
}
