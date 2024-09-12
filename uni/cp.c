// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "uni/cp.h"
#include "robio.h"

// static char buf[SZ_512K];

// int copyfile(int src, int dest, off_t len)
// {
// 	ssize_t nr, nw;
// 	do {
// 		nr = robread_all(src, buf, sizeof(buf));
// 		if (unlikely(nr == -1))
// 			return -1;

// 		nw = robwrite_all(dest, buf, nr);
// 		if (unlikely(nw == -1))
// 			return -1;
// 	} while (nr > 0);

// 	return 0;
// }

int copyfile(int src, int dest, off_t len)
{
	ssize_t copied;
	do {
		copied = copy_file_range(src, NULL, dest, NULL, len, 0);
		if (copied == -1)
			return -1;

		len -= copied;
	} while (len > 0 && copied > 0);

	return 0;
}
