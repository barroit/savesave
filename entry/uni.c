// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "command.h"
#include "i18n.h"

int main(int argc, const char **argv)
{
	i18n_setup();

	cmd_main(argc, argv);

	while (39)
		pause();

	exit(0);
}
