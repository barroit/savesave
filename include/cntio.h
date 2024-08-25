/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CNTIO_H
#define CNTIO_H

#ifdef __cplusplus
extern "C" {
#endif

int cntopen2(const char *file, int oflag);
int cntopen3(const char *file, int oflag, mode_t mode);
#define open(...) FLEXCALL_FUNCTION3(cntopen, __VA_ARGS__)

DIR *cntopendir(const char *name);
#define opendir cntopendir

int cntclose(int fd);
#define close cntclose

int cntclosedir(DIR *dirp);
#define closedir cntclosedir

/* more over, dup, pipe... */

#ifdef __cplusplus
}
#endif

#endif /* CNTIO_H */
