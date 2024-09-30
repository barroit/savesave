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
#include "baksched.h"

LONGRUNNING
USEDOTSAV
UNIQUEPROC
int cmd_backup(int argc, const char **argv)
CMDDESCRIP("Start backup task in background")
{
	baktimer_t tmrctx;
	baksched_t schedctx;

	struct savesave *sav;
	size_t nl = get_dotsav(&sav);

	baksched_init(&schedctx, nl);

	baktimer_init(&tmrctx, schedctx, sav, nl);
	baktimer_arm(tmrctx);
	return 0;
}
