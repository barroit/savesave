/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef SAVCONF_H
#define SAVCONF_H

extern size_t dotsav_size;
/*
 * Do not modify this pointer
 */
extern struct savesave *dotsav_array;

static inline int sav_is_last(struct savesave *sav)
{
	return sav->name == NULL;
}

#define for_each_sav(sav) for (; !sav_is_last(sav); sav++)

size_t dotsav_parse(char *savstr, struct savesave **sav);

void dotsav_print(struct savesave *sav);

#ifdef _WIN32
void dotsav_format(struct savesave *dotsav, size_t n);
#endif

void dotsav_prepare(void);

#endif /* GETCONF_H */
