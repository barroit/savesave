/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma once

struct savesave;

class backup {
	HANDLE *timer;

	HANDLE queue;
	bool use_defque;

public:
	backup(size_t nl);

	~backup();

	void create_backup_task(const struct savesave *);
};
