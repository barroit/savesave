// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * version - Print program version, as well as the library version
 */

#include "argupar.h"

int cmd_version(int argc, const char **argv)
CMDDESCRIP("Display version information about Savesave")
{
	puts(SAVESAVE_NAME "/" SAVESAVE_BUILD "-"
	     SAVESAVE_ARCH " " SAVESAVE_VERSION);

	printf("   libzstd-%s\n", ZSTD_versionString());

#ifdef HAVE_INTL
	printf("   gettext-%s\n", GETTEXT_VERSION);
#endif

	exit(0);
}
