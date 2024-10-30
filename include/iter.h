/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ITER_H
#define ITER_H

#define for_each_idx(i, n) for (i = 0; i < n; i++)

/*
 * handle index up to max_value(i) - 1
 */
#define for_each_idx_back(i, n) for (i = n; i != max_value(i); i--)

#define for_each_idx_from(i, n) for (; i < n; i++)

#define for_each_str(arr) for (; *arr != NULL; arr++)

#endif /* ITER_H */
