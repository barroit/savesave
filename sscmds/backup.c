// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "baktimer.h"
#include "dotsav.h"
#include "maincmd.h"

LONGRUNNING
USEDOTSAV
UNIQUEPROC
int cmd_backup(int argc, const char **argv)
CMDDESCRIP("Start backup task in background")
{
	baktimer_t bt;
	struct savesave *sav;
	size_t nl = get_dotsav(&sav);

	dotsav_print(sav);
	// baktimer_init(&bt, sav, nl);
	// baktimer_arm(&bt);
	return 0;
}
