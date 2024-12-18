/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef BAKTMR_H
#define BAKTMR_H

#define BAKTMR_OVERLOAD_WINDOW sec_to_nanosec(5ULL)
#define BAKTMR_MAX_OVERLOAD    5

void baktmr_arm(void);

void FEATSPEC(bakolc_init)(void);

void FEATSPEC(baktmr_callback)(uint idx, char *rtm);

void baktmr_disarm(void);

#endif /* BAKTMR_H */
