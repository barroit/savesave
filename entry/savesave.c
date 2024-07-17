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

static void handle_option(int argc, char *const *argv)
{
	enum optid res = parse_option(argc, argv);

	if (res == OPT_VERSION || res == OPT_HELP)
		exit(0);
	else if (res == OPT_CONFIG)
		config_path = optarg;
}

int main(int argc, char *const *argv)
{
	if (argc > 1)
		handle_option(argc, argv);

	if (!config_path)
		config_path = get_default_config_path();

	if (!config_path) {
		error("no configuration was provided");
		exit(128);
	}

	const char *strconf = readfile(config_path);
	if (!strconf) {
		error_errno("failed to read config from ‘%s’", config_path);
		exit(128);
	}

	return 0;
}
