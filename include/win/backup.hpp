/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BACKUP_HPP
#define BACKUP_HPP

#include "backup.h"

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

#endif /* BACKUP_HPP */
