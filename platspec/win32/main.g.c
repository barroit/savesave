// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "command.h"
#include "i18n.h"
#include "prog.h"
#include "cnsl.h"
#include "getargv.h"
#include "window.h"
#include "notifyicon.h"
#include "termas.h"
#include "proc.h"

int cm_do_mr;

int WinMain(HINSTANCE app, HINSTANCE _, char *cmdline, int __)
{
	const char *mr = getenv(PROC_DO_MINIMAL);
	cm_do_mr = mr && strcmp(mr, "y") == 0;

	prog_init();
	if (!cm_do_mr)
		cnsl_attach();

	prog_init_ob();
	i18n_setup();

	const char *defv[] = { "savesave", "backup" };
	int argc = sizeof_array(defv);
	const char **argv = defv;

	if (*cmdline)
		argc = getargv(&argv);

	cmd_main(argc, argv);

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
