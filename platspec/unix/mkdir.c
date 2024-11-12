// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "mkdir.h"
#include "termas.h"

int flexremove(const char *name)
{
	int err;

	err = unlink(name);
	if (!err)
		return 0;
	else if (errno == EISDIR)
		return rmdirr(name);

	return warn_errno(ERRMAS_REMOVE_FILE(name));
}
