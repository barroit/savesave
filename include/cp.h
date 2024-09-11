/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CP_H
#define CP_H

#ifdef __cplusplus
extern "C" {
#endif

struct strbuf;

int copyfile(int src, int dest, off_t len);

int get_link_target2(const char *name, struct strbuf *__buf);

#ifdef __cplusplus
}
#endif

#endif /* CP_H */
