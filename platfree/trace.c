// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "trace.h"

struct trace_timer {
	timestamp_t start;
	timestamp_t end;
};

static struct trace_timer timer;

void trace_start_timer(void)
{
	BUG_ON(timer.start);

	timer.start = timestamp();
}

void trace_stop_timer(void)
{
	BUG_ON(!timer.start);

	timer.end = timestamp();
}

void __trace_print_timer(const char *name)
{
	BUG_ON(!timer.start || !timer.end);

	timestamp_t elapse = timer.end - timer.start;

	printf("function `%s' took %" PRIu64 ".%06" PRIu64 "s\n",
	       name, elapse / 1000000000, (elapse % 1000000000) / 1000);

	memset(&timer, 0, sizeof(timer));
}
