// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * version - Print program version, as well as the library version
 */

#include "argupar.h"

int cmd_version(int argc, const char **argv)
CMDDESCRIP("Display version information about Savesave")
{
	puts(SAVESAVE_NAME "/" SAVESAVE_HOST "-"
	     SAVESAVE_ARCH " " SAVESAVE_VERSION);

	printf("   libzstd-%s\n", ZSTD_versionString());

#ifdef HAVE_INTL
	printf("   gettext-%s\n", GETTEXT_VERSION);
#endif

	return 0;
}
