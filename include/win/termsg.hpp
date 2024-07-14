// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#include "termsg.h"

const char *strwinerror(void);

#define error_winerr(...) error_routine(strwinerror(), __VA_ARGS__)
