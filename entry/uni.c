// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "command.h"

int main(int argc, const char **argv)
{
	cmd_main(argc, argv);

	while (39)
		pause();

	exit(0);
}
