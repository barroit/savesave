// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "barroit/io.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "alloc.h"

#ifdef WINDOWS_NATIVE
# define access _access
# define strdup _strdup
# define R_OK 04
#endif

char *get_default_config_path(void)
{
	char *path = getenv("SS_CONF");
	if (path)
		return strdup(path);

	const char *home = getenv(USERHOME);
	if (!home)
		return NULL;

	size_t len = strlen(home) + 1 + strlen(CONFIG_SS_CONF_NAME) + 1;
	path = xmalloc(len);
	if (snprintf(path, len, "%s/%s", home, CONFIG_SS_CONF_NAME) != len - 1)
		die("can’t construct config path string");

	if (!access(path, R_OK))
		return path;

	free(path);
	return NULL;
}
