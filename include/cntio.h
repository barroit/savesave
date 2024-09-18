/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * automatically included by build system
 */

#ifndef CNTIO_H
#define CNTIO_H

#ifdef __cplusplus
extern "C" {
#endif

int cntcreat(const char *file, mode_t mode);
#define creat cntcreat

int cntopen2(const char *file, int oflag);
int cntopen3(const char *file, int oflag, mode_t mode);
#define open(...) FLEXCALL_FUNCTION3(cntopen, __VA_ARGS__)

int cntclose(int fd);
#define close cntclose

#ifdef __linux__

DIR *cntopendir(const char *name);
#define opendir cntopendir

int cntclosedir(DIR *dirp);
#define closedir cntclosedir

#endif /* __linux__ */

int cntdup2(int oldfd, int newfd);
#define dup2 cntdup2

/* more over, dup, pipe... */

#ifdef __cplusplus
}
#endif

#endif /* CNTIO_H */
