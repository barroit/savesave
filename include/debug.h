/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef NDEBUG

# define DEBUG_RUN() if (0)

# define BUG_ON(cond) do {} while (0)

#else /* debugging */

# define DEBUG_RUN() if (1)

# define BUG_ON(cond) assert(!(cond))

#endif

#endif /* DEBUG_H */
