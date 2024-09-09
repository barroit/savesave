// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "keepref.h"

int setenv(const char *name, const char *value, int overwrite)
{
	if (!name || *name == 0 || strchr(name, '='))
		goto err_nul_name;

	char *prev = getenv(name);
	if (prev && !overwrite)
		return 0;

	size_t nalen = strlen(name);
	size_t valen = strlen(value);
	size_t stlen = nalen + 1 + valen;

	char *str = malloc(stlen + 1);
	if (!str)
		goto err_no_mem;

	memcpy(str, name, nalen);
	str[nalen] = '=';
	memcpy(&str[nalen + 1], value, valen);
	str[stlen] = 0;

	int err = putenv(str);
	if (err)
		goto err_put_env;

	KEEPREF(str);
	return 0;

err_put_env:
	free(str);
	return -1;
err_nul_name:
	errno = EINVAL;
	return -1;
err_no_mem:
	errno = ENOMEM;
	return -1;
}
