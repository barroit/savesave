/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG

# define DEBUG_RUN() if (0)

# define BUG_ON(cond) do { if (cond) {} } while (0)

#else /* debugging */

# define DEBUG_RUN() if (1)

# define BUG_ON(cond) assert(!(cond))

#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
