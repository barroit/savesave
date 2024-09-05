/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#include "termas.h"

const char *strwinerror();

#define warn_winerr(...) \
	__warn_routine("warning: ", strwinerror(), __VA_ARGS__)

#define error_winerr(...) \
	__error_routine("error: ", strwinerror(), __VA_ARGS__)

#define die_winerr(...) __die_routine("fatal: ", strwinerror(), __VA_ARGS__)
