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

struct savesave {
	char *name;

	char *save_prefix;
	char *backup_prefix;

	off64_t save_size;
	int is_dir_save;

	int use_compress;
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
