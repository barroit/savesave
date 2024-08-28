/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "calc.h"

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

#define for_each_idx(i, n) for (i = 0; i < n; i++)

/*
 * for_each_idx_back - for each index backward
 * i: the index variable
 * n: the value assigned to i
 * 
 * note: this for loop can handle index up to max_uint_val_of_type(i)
 */
#define for_each_idx_back(i, n) \
	for (i = n; i != max_uint_val_of_type(i); i--)

#define for_each_idx_from(i, n) for (; i < n; i++)

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
