// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/termsg.hpp"
#include "calc.h"

static char errbuf[256];

const char *strwinerror()
{
	int errnum = GetLastError();
	int langid = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		      NULL, errnum, langid, errbuf,
		      ARRAY_SIZEOF(errbuf), NULL);

	errno = errnum;
	return errbuf;
}
