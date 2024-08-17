/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef SAVCONF_H
#define SAVCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "barroit/types.h"

struct savesave {
	const char *name;

	const char *save;
	off_t save_size;
	int is_dir_save;

	const char *backup;

	int use_zip;
	int use_snapshot;

	u32 period;
	u8 stack;
};

char *get_default_savconf_path(void);

size_t parse_savconf(const char *path, struct savesave **conf);

void print_savconf(const struct savesave *conf, size_t nl);

#ifdef __cplusplus
}
#endif

#endif /* GETCONF_H */
