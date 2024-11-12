// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "constructor.h"
#include "termas.h"

USESTDIO
CONSTRUCTOR(check_os_version)
{
	if (!IsWindows7OrGreater())
		die(_("unsupported windows version (at least windows 7)"));
}
