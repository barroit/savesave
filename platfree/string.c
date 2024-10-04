// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termas.h"
#include "calc.h"

void strrepl(char *str, int c, int r)
{
	while ((str = strchr(str, c)) != NULL)
		*str++ = r;
}

char *straftr(const char *str, const char *pref)
{
	do {
		if (!*pref)
			return (char *)str;
	} while (*str++ == *pref++);

	return NULL;
}

int strskip(const char *str, const char *pref, const char **rest)
{
	char *p = straftr(str, pref);
	if (!p)
		return -1;

	*rest = p;
	return 0;
}

char *strskipws(const char *str)
{
	while (isspace(*str))
		str++;
	return (char *)str;
}

char *strtrimend(char *str)
{	
	size_t len = strlen(str);
	char *tail = &str[len];

	while (tail != str && isspace(*(tail - 1)))
		tail--;
	*tail = 0;
	return (char *)str;
}

char *strnxtws(const char *str)
{
	while (*str && !isspace(*str))
		str++;
	return *str == 0 ? NULL : (char *)str;
}

int str2llong(const char *str, size_t len, llong *value, llong min, llong max)
{
	BUG_ON(!*str);
	if (isspace(*str))
		goto err_inv_char;

	if (len == SIZE_MAX)
		len = strlen(str);

	char *end;
	errno = 0;
	llong val = strtoll(str, &end, 10);

	if ((val == LLONG_MIN || val == LLONG_MAX) && errno == ERANGE)
		goto err_out_of_range;
	else if (end - str != len)
		goto err_inv_char;
	else if (val > max || val < min)
		goto err_out_of_range;

	*value = val;
	return 0;

err_out_of_range:
	return warn(_("signed integer `%s' is too big (range [%lld, %lld])"),
		    str, min, max);
err_inv_char:
	return warn(_("invalid character found in signed value `%s'"), str);
}

int str2ullong(const char *str, size_t len, ullong *value, ullong max)
{
	BUG_ON(!*str);
	if (isspace(*str))
		goto err_inv_char;

	if (len == SIZE_MAX)
		len = strlen(str);

	char *end;
	errno = 0;
	ullong val = strtoull(str, &end, 10);

	if (val == ULLONG_MAX && errno == ERANGE)
		goto err_out_of_range;
	else if (end - str != len)
		goto err_inv_char;
	else if (val > max)
		goto err_out_of_range;

	*value = val;
	return 0;

err_out_of_range:
	return warn(_("unsigned integer `%s' is too big (up to %llu)"),
		    str, max);
err_inv_char:
	return warn(_("invalid character found in unsigned integer `%s'"),
		    str);
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

int str2timespan(const char *str, u32 *value)
{
	int err;

	u16 mult = 1;
	size_t len = strlen(str);
	char unit = str[len - 1];

	if (!isdigit(unit)) {
		err = char2mult(unit, &mult);
		if (err)
			goto err_unknown_unit;
		len -= 1;
	}

	ullong val;
	err = str2ullong(str, len, &val, UINT32_MAX);
	if (err)
		return err;

	*value = st_mult(val, mult);
	return 0;

err_unknown_unit:
	return error(_("unknown time unit specifier `%c'"), unit);
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
