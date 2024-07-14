// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
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

#ifdef __cplusplus
}
#endif

#endif /* COMPAT_H */
