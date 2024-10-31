// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

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

	strlist_init(&sl, STRLIST_STORE_REF);
	for_each_idx(i, argc) {
		const wchar_t *arg = wargv[i];
		size_t size = wcsrtombs(NULL, &arg, 0, NULL);

		if (size == SIZE_MAX)
			die_errno(_("failed to convert wide character string to multibyte string"));
		size += 1;

		char *buf = xmalloc(size);
		arg = wargv[i];

		size = wcsrtombs(buf, &arg, size, NULL);
		BUG_ON(size == SIZE_MAX || arg);

		strlist_push(&sl, buf);
	}

	*argv = (const char **)strlist_dump2(&sl, 0);

	LocalFree(wargv);
	strlist_destroy(&sl);
	return argc;
}
