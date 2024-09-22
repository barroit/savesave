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

int WinMain(HINSTANCE app, HINSTANCE _, char *cmdline, int ___)
{
	do_setup();
	setup_console();
	do_delayed_setup();

	const char *defargv[] = { "savesave", "start" };
	int argc = sizeof_array(defargv);
	const char **argv = defargv;

	if (*cmdline)
		argc = getargv(&argv);

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




// backup::backup(size_t nl)
// {
// 	timer = new HANDLE[nl];
// 	queue = CreateTimerQueue();

// 	use_defque = !queue;
// 	if (use_defque)
// 		warn_winerr(_("failed to create timer queue, fallback to default one"));
// }

// backup::~backup()
// {
// 	delete[] timer;

// 	if (!use_defque)
// 		DeleteTimerQueue(queue);
// }

// static void do_backup(void *conf, unsigned char)
// {
// 	int err;
// 	struct savesave *c = (struct savesave *)conf;

// 	err = backup(c);
// 	if (!err)
// 		error(_("cannot make backup for configuration `%s'"), c->name);
// }

// void backup::create_backup_task(const struct savesave *conf)
// {
// 	int err;
// 	HANDLE *p = timer;

// 	while (conf->name != NULL) {
// 		DWORD countdown = conf->period * 1000;
// 		ULONG flag = conf->save_size > CONFIG_COMPRESSING_THRESHOLD ?
// 				WT_EXECUTELONGFUNCTION : WT_EXECUTEDEFAULT;

// 		err = !CreateTimerQueueTimer(p++, queue, do_backup,
// 					     (void *)conf++, countdown,
// 					     countdown, flag);
// 		if (err)
// 			goto err_create_timer;
// 	}

// err_create_timer:
// 	die_winerr(_("failed to create timer for configuration `%s'"),
// 		   conf->name);
// }
