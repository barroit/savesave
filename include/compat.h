// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 * 
 * This file shall only be include by build system
 */

#ifndef COMPAT_H
#define COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if (defined __clang__)
# if (defined _WIN32)
#  include <BaseTsd.h>
#  include <stdint.h>
typedef SSIZE_T ssize_t;
#  define SSIZE_MAX INT64_MAX
# else
#  include <stdio.h>
# endif
#endif

#if (defined __clang__ && defined _WIN32)
# define WINDOWS_NATIVE
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPAT_H */
