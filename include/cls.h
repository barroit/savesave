/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CLS_H
#define CLS_H

typedef void (*cls_callback_t)(void);

void cls_push(cls_callback_t cb);

cls_callback_t cls_pop(void);

void cls_apply(void);

#endif /* CLS_H */
