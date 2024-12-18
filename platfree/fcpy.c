// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "fcpy.h"
#include "fileiter.h"
#include "strbuf.h"
#include "termas.h"

int __fiter_cpy_nonreg(struct iterfile *src, void *data)
{
	struct strbuf *dest = &(((struct strbuf *)data)[0]);
	strbuf_boconcat(dest, src->dymname);

	if (S_ISLNK(src->st.st_mode))
		return copy_symlink(src->absname, dest->str);

	int err = MKDIR(dest->str);
	if (unlikely(err))
		return warn_errno(ERRMAS_CREAT_DIR(dest->str));

	return 0;
}
