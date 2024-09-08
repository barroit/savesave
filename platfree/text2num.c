// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "text2num.h"
#include "termas.h"
#include "calc.h"

int str2ulong(const char *str, size_t len,
	      unsigned long max, unsigned long *val)
{
	if (isspace(*str) || *str == '-')
		return error(_("unexpected leading char found in `%s'"), str);

	char *tail;

	errno = 0;
	*val = strtoul(str, &tail, 10);

	if (errno != 0)
		return error_errno(_("failed to convert `%s'"), str);
	else if (tail - str != len)
		return error(_("invalid char found in `%s'"), str);
	else if (*val > max)
		return error(_("`%s' is too big (up to %lu)"), str, max);

	return 0;
}

static int char2mult(char c, u16 *mult)
{
	*mult = 1;
	switch (c) {
	case 'H':
	case 'h':
		*mult *= 60;
	case 'M':
	case 'm':
		*mult *= 60;
	case 'S':
	case 's':
		return 0;
	}

	return 1;
}

int str2period(const char *str, u32 *val)
{
	int err;

	size_t len = strlen(str);
	u16 mult = 1;
	char lc = str[len - 1];

	if (!isdigit(lc)) {
		err = char2mult(lc, &mult);
		if (err)
			return error(_("unknown time unit specifier `%c'"),
				     lc);
		len -= 1;
	}

	err = str2ulong(str, len, UINT32_MAX, (unsigned long *)val);
	if (err)
		return err;

	*val = st_mult(*val, mult);
	return 0;
}

int str2bool(const char *str, int *val)
{
	if (str[1] != 0)
		goto err_unknown_value;
	else if (*str == 'y')
		*val = 1;
	else if (*str == 'n')
		*val = 0;
	else
		goto err_unknown_value;

	return 0;

err_unknown_value:
		return error(_("unknown bool value `%s'"), str);
}
