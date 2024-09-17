// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "termas.h"
#include "dotsav.h"
#include "debug.h"
#include "backup.h"

static const char *dotsav_path;

// static struct arguopt options[] = {
// 	APOPT_FILENAME(),
// 	APOPT_END(),
// };

int main(int argc, const char **argv)
{
	setup_message_translation();

	if (!dotsav_path)
		dotsav_path = get_dotsav_defpath();
	if (!dotsav_path)
		die(_("no dotsav (.savesave) was provided"));

	struct savesave *savv;
	size_t savc = parse_dotsav(dotsav_path, &savv);
	if (!savc)
		die(_("dotsav `%s' must contain at least one configuration"),
		    dotsav_path);
	DEBUG_RUN()
		print_dotsav(savv, savc);

	backup(savv);
	exit(0);
}
