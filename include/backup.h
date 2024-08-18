/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BACKUP_H
#define BACKUP_H

#ifdef __cplusplus
extern "C" {
#endif

struct savesave;

int backup_routine(const struct savesave *c);

#ifdef __cplusplus
}
#endif

#endif /* BACKUP_H */
