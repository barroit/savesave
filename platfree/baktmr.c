// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktmr.h"
#include "strbuf.h"
#include "termas.h"
#include "dotsav.h"
#include "mkdir.h"
#include "path.h"
#include "iter.h"
#include "proc.h"

int FEATSPEC(baktmr_callback)(uint idx)
{
	struct savesave *sav = &dotsav_array[idx];
	const char *exec = exec_path();
	const char *output = output_path();
	struct proc ps = {
		.flags = PROC_RD_STDIN | PROC_RD_STDOUT | PROC_RD_STDERR,
	};

	int err;
	if (sav->period > CONFIG_AIO_COPY_THRESHOLD)
		err = proc_exec(&ps, exec, exec,
				"--output", output,
				"copy", "--asynced", sav->name, NULL);
	else
		err = proc_exec(&ps, exec, exec,
				"--output", output, "copy", sav->name, NULL);
	if (err)
		return error(_("can't backup sav `%s'"), sav->name);

	int ret;
	err = proc_wait(&ps, &ret);
	if (err)
		return warn_errno(_("failed finish backup routine for sav `%s'"),
				  sav->name);

	if (ret)
		return warn(_("backup routine of sav `%s' eixted with code `%d'"),
			    sav->name, ret);
	return 0;
}
