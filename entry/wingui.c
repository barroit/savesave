// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
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

int WinMain(HINSTANCE app, HINSTANCE _, char *cmdline, int __)
{
	prog_init();
	cnsl_attach();
	prog_init_ob();

	i18n_setup();

	const char *defargv[] = { "savesave", "backup" };
	int argc = sizeof_array(defargv);
	const char **argv = defargv;

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
