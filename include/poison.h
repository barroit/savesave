/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef POSION_H
#define POSION_H

#define ACCESS_POISON ((void *) 0xdeadbeef)

#define LIST_POISON1 ((void *) 0x100)
#define LIST_POISON2 ((void *) 0x200)

#define STRBUF_POISON ((void *) 0x300)

#define STRLIST_POISON ((void *) 0x400)

#define CALLBACK_MAP_POISON1 ((void *) 0x500)
#define CALLBACK_MAP_POISON2 ((void *) 0x600)

#endif /* POSION_H */
