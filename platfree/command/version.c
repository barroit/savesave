// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * version - Print program version, as well as the library version
 */

int cmd_version(int argc, const char **argv)
{
	printf("%s/%s-%s %s\n", SAVESAVE_NAME,
	       SAVESAVE_HOST, SAVESAVE_ARCH, SAVESAVE_VERSION);
	printf("   libzstd-%s\n", ZSTD_versionString());
	printf("   gettext-%s\n", GETTEXT_VERSION);

	return 0;
}
