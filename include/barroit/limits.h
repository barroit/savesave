// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BR_LIMIT_H
#define BR_LIMIT_H

#include <limits.h>

/*
 * Most functions use SSIZE_MAX as the portable maximum number of bytes they
 * can write.
 */
#define MAX_IO_SIZE_DEFAULT (8 * 1024 * 1024)
#if (SSIZE_MAX < MAX_IO_SIZE_DEFAULT)
# define MAX_IO_SIZE SSIZE_MAX
#else
# define MAX_IO_SIZE MAX_IO_SIZE_DEFAULT
#endif

#endif /* BR_LIMIT_H */
