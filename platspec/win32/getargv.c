// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

/*
 * dumb clang
 */
#pragma GCC diagnostic ignored "-Wreturn-type"

#include "getargv.h"
#include "strlist.h"
#include "iter.h"
#include "termas.h"
#include "alloc.h"

int getargv(const char ***argv)
{
	int argc;
	wchar_t *wargs = GetCommandLineW();
	wchar_t **wargv = CommandLineToArgvW(wargs, &argc);

	if (!wargv)
		die_winerr("failed to interpret command argument");

	size_t i;
	struct strlist sl;

	strlist_init(&sl, STRLIST_USEREF);
	for_each_idx(i, argc) {
		const wchar_t *arg = wargv[i];
		size_t size = wcsrtombs(NULL, &arg, 0, NULL);

		if (size == SIZE_MAX)
			goto err_wstr2mstr;
		size += 1;

		char *buf = xmalloc(size);
		arg = wargv[i];

		size = wcsrtombs(buf, &arg, size, NULL);
		BUG_ON(size == SIZE_MAX || arg);

		strlist_push(&sl, buf);
	}

	*argv = (const char **)strlist_dump(&sl);

	LocalFree(wargv);
	strlist_destroy(&sl);
	return argc;

err_wstr2mstr:
	die_errno("failed to convert wide character string to multibyte string");
}
