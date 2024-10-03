/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_RESOURCE_H
#define BRT_RESOURCE_H

rlim_t getfdavail(void);

uint getcpucores(void);

#endif /* BRT_RESOURCE_H */
