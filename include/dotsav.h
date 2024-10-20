/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef SAVCONF_H
#define SAVCONF_H

static inline int sav_is_last(struct savesave *sav)
{
	return sav->name == NULL;
}

#define for_each_sav(sav) for (; !sav_is_last(sav); sav++)

char *read_dotsav(const char *name);

size_t dotsav_parse(char *savstr, struct savesave **sav);

void dotsav_print(struct savesave *sav);

#ifdef _WIN32
void format_dotsav(struct savesave *dotsav, size_t n);
#endif

#endif /* GETCONF_H */
