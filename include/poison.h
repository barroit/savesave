/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef POSION_H
#define POSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define GENERIC_POISON ((void *) 0xdeadbeef)

#define LIST_POISON1 ((void *) 0x100)
#define LIST_POISON2 ((void *) 0x200)

#define STRBUF_POISON ((void *) 0x300)

#define STRLIST_POISON ((void *) 0x400)

#ifdef __cplusplus
}
#endif

#endif /* POSION_H */
