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

	off_t save_size;
	int is_dir_save;

	int use_compress;
	int use_snapshot;

	u32 period;
	u8 stack;
};

char *get_dotsav_defpath(void);

struct fileiter_file;
int PLATSPECOF(sizeof_file)(struct fileiter_file *file, void *data);

size_t parse_dotsav(const char *path, struct savesave **sav);

void print_dotsav(struct savesave *sav, size_t nl);

#ifdef _WIN32
void format_dotsav(struct savesave *dotsav, size_t n);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GETCONF_H */
