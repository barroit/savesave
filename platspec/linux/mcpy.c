// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mcpy.h"
#include "fileiter.h"
#include "cpsched.h"
#include "acpy.h"
#include "fcpy.h"
#include "strbuf.h"
#include "termas.h"

int mcpy_disabled;

void mcpy_deploy(uint qs, size_t bs, uint ts)
{
	cpsched_deploy(ts);
	acpy_deploy(qs, bs);

	if (!acpy_disabled)
		return;

	mcpy_disabled = 1;
}

static int cp_reg_and_unsuppd(struct iterfile *src, void *data)
{
	struct strbuf *dest = data;
	strbuf_concatat_base(dest, src->dymname);

	int ret = cpsched_schedule(src->absname, dest->str);
	if (ret != SCHED_BUSY)
		return ret;

	return __acpyreg(src->absname, dest->str);
}

int mcpy_copy(const char *src, const char *dest)
{
	int ret;
	struct strbuf data = strbuf_from2(dest, 0, PATH_MAX);

	ret = fileiter(src, __fiter_cpy_nonreg, &data, __FITER_CPY_NONREG);
	if (ret)
		goto err_cpy;

	ret = fileiter(src, cp_reg_and_unsuppd, &data, FITER_NO_SYMLINK);
	if (ret)
		goto err_cpy;

	if (acpy_remains()) {
		ret = acpy_comp_cqe(ACPY_FULLCOMP);
		if (ret)
			acpy_drop_entries();
	}

	if (ret) {
	err_cpy:
		warn(ERRMAS_COPY_FILE(src, dest));
	}

	strbuf_destroy(&data);
	return ret;
}
