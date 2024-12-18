/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef BRT_LIMIT_H
#define BRT_LIMIT_H

/*
 * Most functions use SSIZE_MAX as the portable maximum number of bytes they
 * can write.
 */
#define MAX_IO_SIZE ((size_t)(__MAX_IO_SIZE + 0LLU))
#if (SSIZE_MAX < MAX_IO_SIZE_DEFAULT)
# define __MAX_IO_SIZE SSIZE_MAX
#else
# define __MAX_IO_SIZE (8 * 1024 * 1024)
#endif

#define MAX_ALLOC_SIZE ((size_t)(__MAX_ALLOC_SIZE + 0LLU))
#if (SIZE_MAX < MAX_ALLOC_SIZE_DEFAULT)
# define __MAX_ALLOC_SIZE SIZE_MAX
#else
# define __MAX_ALLOC_SIZE (32 * 1024 * 1024)
#endif

/*
 * number of character in a uint8 string including the terminating null byte
 */
#define STRU8_MAX 4

/*
 * number of character in a pid string including the terminating null byte
 */
#define STRPID_MAX 11

#ifdef _WIN32
# define SSIZE_MAX INT64_MAX
# define PATH_MAX MAX_PATH
#endif

#endif /* BRT_LIMIT_H */
