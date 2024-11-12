/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef BRT_RESOURCE_H
#define BRT_RESOURCE_H

rlim_t getfdavail(void);

uint getcpucores(void);

#endif /* BRT_RESOURCE_H */
