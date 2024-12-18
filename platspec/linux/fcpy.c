// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "fcpy.h"

int FEATSPEC(fd2fd_copy)(int src, int dest, off_t len)
{
	BUG_ON(!len);

	ssize_t copied;
	do {
		copied = copy_file_range(src, NULL, dest, NULL, len, 0);
		if (copied == -1)
			return -1;

		len -= copied;
	} while (len > 0 && copied > 0);

	return 0;
}
