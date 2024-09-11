// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "alloc.h"
#include "strbuf.h"
#include "strlist.h"

void fileiter_init(struct fileiter *ctx, const char *root,
		    fileiter_callback cb, void *data)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->root = root;

	ctx->sb = xmalloc(sizeof(*ctx->sb));
	strbuf_init(ctx->sb, 0);

	ctx->sl = xmalloc(sizeof(*ctx->sl));
	strlist_init(ctx->sl, 0);

	ctx->cb = cb;
	ctx->data = data;
}

void fileiter_destroy(struct fileiter *ctx)
{
	strbuf_destroy(ctx->sb);
	strlist_destroy(ctx->sl);

	free(ctx->sb);
	free(ctx->sl);
}

int fileiter_do_exec(struct fileiter *ctx);

int fileiter_exec(struct fileiter *ctx)
{
	int ret;
	const char *dir = ctx->root;

	do {
		strbuf_reset_from(ctx->sb, dir);

		ret = fileiter_do_exec(ctx);
		if (ret)
			return -1;

	} while ((dir = strlist_pop2(ctx->sl, 0)));

	return 0;
}
