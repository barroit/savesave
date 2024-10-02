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

	int is_dir_save;
	int use_compress;

	u32 period;
	u8 stack;

	uint task_idx;
	ullong task_pos;
};

#define for_each_sav(sav) for (; sav->name; sav++)

char *read_dotsav(const char *name);

size_t dotsav_parse(char *savstr, struct savesave **sav);

void dotsav_print(struct savesave *sav);

#ifdef _WIN32
void format_dotsav(struct savesave *dotsav, size_t n);
#endif

#endif /* GETCONF_H */
