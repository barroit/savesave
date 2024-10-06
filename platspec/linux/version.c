// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "constructor.h"
#include "termas.h"

CONSTRUCTOR(check_kernel_version)
{
	struct utsname un;
	uname(&un);

	int err;
	const char *str = un.release;
	ullong version, patchlevel;
	const char *sep = strchr(str, '.');

	if (!sep)
		goto err_malformed;
	err = str2ullong(str, sep - str, &version, -1);
	if (err)
		goto err_malformed;
	if (version < 5)
		goto err_too_old;
	else if (version > 5)
		return;

	str = sep + 1;
	sep = strchr(str, '.');

	if (!sep)
		goto err_malformed;
	err = str2ullong(str, sep - str, &patchlevel, -1);
	if (err)
		goto err_malformed;
	if (patchlevel < 6)
		goto err_too_old;

	return;

err_malformed:
	die(_("kenel version `%s' is malformed"), un.release);
err_too_old:
	die(_("kernel version `%s' is too old (minimum 5.6)"), un.release);
}
