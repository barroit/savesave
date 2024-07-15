// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include <stdlib.h>

static void handle_option(int argc, char *const *argv)
{
	int res = parse_option(argc, argv);

	if (res == OPT_VERSION || res == OPT_HELP)
		exit(0);
}

int main(int argc, char *const *argv)
{
	if (argc > 1)
		handle_option(argc, argv);

	return 0;
}
