/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * automatically included by build system
 */

#ifndef CNTIO_H
#define CNTIO_H

extern rlim_t cntio_fdcnt;

int cntcreat(const char *file, mode_t mode);
#define creat cntcreat

#ifdef _WIN32
/*
 * good job, microsoft filesystem permission strategy
 */
# define flexcreat(file) creat(file, S_IREAD | S_IWRITE)
#else
# define flexcreat(file) creat(file, 0664)
#endif

int cntopen2(const char *file, int oflag);
int cntopen3(const char *file, int oflag, mode_t mode);
#define open(...) FLEXCALL_FUNCTION3(cntopen, __VA_ARGS__)

int cntclose(int fd);
#define close cntclose

#ifdef __unix__

DIR *cntopendir(const char *name);
#define opendir cntopendir

int cntclosedir(DIR *dirp);
#define closedir cntclosedir

int cntpipe(int pipedes[2]);
#define pipe cntpipe

#endif /* __unix__ */

int cntdup(int oldfd);
#define dup cntdup

int cntdup2(int oldfd, int newfd);
#define dup2 cntdup2

/* more over, dup, pipe... */

#endif /* CNTIO_H */
