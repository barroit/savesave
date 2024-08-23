/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

class atenter {
public:
	atenter();

	/**
	 * precheck - perform prechecks that should be done by constructor
	 * 	      funcitons on linux
	 */
	void precheck();
};
