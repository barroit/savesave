// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

#include <stddef.h>

template <typename T>
T* xnew();

template <typename T>
T* xnew(size_t n);
