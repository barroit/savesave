/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * A wrapper for atexit(3) that guarantees the cleanup functions are called in
 * the order they were added.
 */

#ifndef CLS_H
#define CLS_H

typedef void (*cls_callback_t)(void);

void cls_add(cls_callback_t cb);

cls_callback_t cls_rm(void);

#endif /* CLS_H */
