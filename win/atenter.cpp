// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/atenter.hpp"
#include "termas.h"
#include "win/dumpstack.hpp"

extern "C" {
CONSTRUCTOR(check_libzstd_mt);
CONSTRUCTOR(init_u8tstr_table);
CONSTRUCTOR(populate_executable_dir);
}

atenter::atenter()
{
	init_u8tstr_table();
	setup_crt_report_hook();
	populate_executable_dir();
}

static void check_os_version()
{
	if (!IsWindows7OrGreater())
		die(_("unsupported windows version (at least win7)"));
}

void atenter::precheck()
{
	/*
	 * win32 gui app does not output to the console from which it was
	 * launched; we need to defer checks and execute them only after the
	 * console has been initialized
	*/
	check_os_version();
	check_libzstd_mt();
}
