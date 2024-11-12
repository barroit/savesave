/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef CLS_H
#define CLS_H

typedef void (*cls_callback_t)(void);

void cls_push(cls_callback_t cb);

cls_callback_t cls_pop(void);

#endif /* CLS_H */
