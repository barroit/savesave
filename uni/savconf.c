// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"

int PLATSPECOF(sizeof_file)(struct fileiter_file *file, void *data)
{
	*((off_t *)data) += file->st->st_size;
	return 0;
}
