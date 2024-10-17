/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CPSCHED_H
#define CPSCHED_H

void cpsched_deploy(size_t nl);

int cpsched_schedule(const char *src, const char *dest);

void cpsched_join(int *res);

#endif /* CPSCHED_H */
