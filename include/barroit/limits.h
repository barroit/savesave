/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_LIMIT_H
#define BRT_LIMIT_H

/*
 * Most functions use SSIZE_MAX as the portable maximum number of bytes they
 * can write.
 */
#define MAX_IO_SIZE_DEFAULT ((8 * 1024 * 1024) + 0U)
#if (SSIZE_MAX < MAX_IO_SIZE_DEFAULT)
# define MAX_IO_SIZE SSIZE_MAX
#else
# define MAX_IO_SIZE MAX_IO_SIZE_DEFAULT
#endif

#define MAX_ALLOC_SIZE_DEFAULT ((32 * 1024 * 1024) + 0U)
#if (SIZE_MAX < MAX_ALLOC_SIZE_DEFAULT)
# define MAX_ALLOC_SIZE SIZE_MAX
#else
# define MAX_ALLOC_SIZE MAX_ALLOC_SIZE_DEFAULT
#endif

/*
 * chars in a string uint8 number including nul
 */
#define STRU8_MAX 4

#endif /* BRT_LIMIT_H */
