// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "termas.h"

int PLATSPECOF(flexremove)(const char *name)
{
	int err;

	err = unlink(name);
	if (!err)
		return 0;
	else if (errno == EISDIR)
		return rmdirr(name);

	return warn_errno(_("failed to remove file `%s'"), name);
}
