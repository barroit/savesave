// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "atenter.h"
#include "console.h"
#include "getargv.h"
#include "window.h"
#include "notifyicon.h"
#include "maincmd.h"
#include "termas.h"
#include "atexit.h"
#include "proc.h"

static void __prepare_longrunning(void)
{
	setup_lr_logging();
	atexit_enque(teardown_lr_logging);

	push_process_id();
	atexit_enque(pop_process_id);

	atexit_apply();
}

int WinMain(HINSTANCE app, HINSTANCE _, char *cmdline, int __)
{
	setup_program();
	setup_console();
	post_setup_program();

	const char *defargv[] = { "savesave", "backup" };
	int argc = sizeof_array(defargv);
	const char **argv = defargv;

	if (*cmdline)
		argc = getargv(&argv);

	prepare_longrunning = __prepare_longrunning;
	cmd_main(argc, argv);

	if (!is_longrunning)
		exit(39);

	HWND window = create_main_window(app);

	NOTIFYICONDATA icon = notifyicon_init(app, window);
	notifyicon_show(&icon);

	MSG mas;
	while (GetMessage(&mas, NULL, 0, 0) > 0) {
		TranslateMessage(&mas);
		DispatchMessage(&mas);
	}

	exit(0);
}
