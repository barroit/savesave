// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef GETCONF_H
#define GETCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_CONFIG_ENV "SAVESAVE"
#define DEFAULT_CONF_NAME  ".savesave"

#include "barroit/types.h"

struct savesave {
	char *save;
	char *backup;

	int use_snapshot;
	int use_zip;

	u32 interval;
	u8 stack;
};

PLATSPEC const char *get_home_dir(void);

PLATSPEC int parse_save_path(const char *path, struct savesave *conf);

PLATSPEC int parse_backup_path(const char *path, struct savesave *conf);

int parse_savesave_config(const char *path, struct savesave *conf);

#ifdef __cplusplus
}
#endif

#endif /* GETCONF_H */
