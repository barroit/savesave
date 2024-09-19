/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef SAVCONF_H
#define SAVCONF_H

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

char *read_dotsav(const char *name);

size_t parse_dotsav(char *savstr, struct savesave **sav);

void print_dotsav(struct savesave *sav, size_t nl);

#ifdef _WIN32
void format_dotsav(struct savesave *dotsav, size_t n);
#endif

#endif /* GETCONF_H */
