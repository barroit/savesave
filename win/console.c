// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termas.h"

USESTDIO CONSTRUCTOR(setup_console_codepage)
{
	int  err = !SetConsoleOutputCP(CP_UTF8);
	if (err)
		warn(_("failed to set console output codepage to 65001"));

	/* SetConsoleCP for input codepage? */
}
