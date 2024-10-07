// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "constructor.h"
#include "termas.h"

#define MIN_VERSION    5
#define MIN_PATCHLEVEL 19

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
	if (version < MIN_VERSION)
		goto err_too_old;
	else if (version > MIN_VERSION)
		return;

	/*
	 * Check patch level for version equal to MIN_VERSION.
	 */
	str = sep + 1;
	sep = strchr(str, '.');

	if (!sep)
		goto err_malformed;
	err = str2ullong(str, sep - str, &patchlevel, -1);
	if (err)
		goto err_malformed;
	if (patchlevel < MIN_PATCHLEVEL)
		goto err_too_old;

	return;

err_malformed:
	die(_("kenel version `%s' is malformed"), un.release);
err_too_old:
	die(_("kernel version `%s' is too old (minimum 5.6)"), un.release);
}
