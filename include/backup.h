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
int backup(struct savesave *c);

struct strbuf;
struct fileiter_file;
int PLATSPECOF(backup_copy_regfile)(struct fileiter_file *src,
				    struct strbuf *dest);
int PLATSPECOF(backup_copy_symlink)(struct fileiter_file *src,
				    struct strbuf *dest, struct strbuf *__buf);

#ifdef __cplusplus
}
#endif

#endif /* BACKUP_H */
