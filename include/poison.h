/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef POSION_H
#define POSION_H

#ifdef CONFIG_USE_POISON

#define ACCESS_POISON ((void *) 0xdeadbeef)

#define LIST_POISON1 ((void *) 0x100)
#define LIST_POISON2 ((void *) 0x200)

#define STRBUF_POISON ((void *) 0x300)

#define STRLIST_POISON ((void *) 0x400)

#define CALLBACK_MAP_POISON1 ((void *) 0x500)
#define CALLBACK_MAP_POISON2 ((void *) 0x600)

#else

#define ACCESS_POISON NULL

#define LIST_POISON1 NULL
#define LIST_POISON2 NULL

#define STRBUF_POISON NULL

#define STRLIST_POISON NULL

#define CALLBACK_MAP_POISON1 NULL
#define CALLBACK_MAP_POISON2 NULL

#endif

#endif /* POSION_H */
