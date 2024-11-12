// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "baktmr.h"
#include "strbuf.h"
#include "termas.h"
#include "dotsav.h"
#include "mkdir.h"
#include "path.h"
#include "iter.h"
#include "proc.h"
#include "barrier.h"

/* overload counter */
struct bakolc {
	uint head;
	uint tail;
	uint mask;
	timestamp_t arr[1 << 3];

	int killed;
	mtx_t lock;
};

static struct bakolc olc = {
	.mask = sizeof_array(olc.arr) - 1,
};

void FEATSPEC(bakolc_init)(void)
{
	mtx_init(&olc.lock, mtx_plain);
}

static void baktmr_callback_busy(uint idx)
{
	timestamp_t new = timestamp();
	timestamp_t old;

	mtx_lock(&olc.lock);
	if (olc.killed) {
		mtx_unlock(&olc.lock);
		return;
	}

	uint tail = olc.tail;
	uint head = olc.head;

	if (tail == head)
		goto push_ts;

	while (head != tail) {
		old = olc.arr[head & olc.mask];
		if (new - old <= BAKTMR_OVERLOAD_WINDOW)
			break;

		head += 1;
	}

	olc.head = head;
	if (head == tail)
		goto push_ts;

	if (tail + 1 - head > BAKTMR_MAX_OVERLOAD) {
		olc.killed = 1;
		mtx_unlock(&olc.lock);

		error(_("too many backup requests were dropped, "
		      "savesave can't keep up anymore"));
		pid_kill(0, SIGTERM);
	}

push_ts:
	olc.arr[tail & olc.mask] = new;
	tail += 1;

	olc.tail = tail;
	mtx_unlock(&olc.lock);

	warn(_("backup request of sav `%s' was dropped"),
	     dotsav_array[idx].name);
}

void FEATSPEC(baktmr_callback)(uint idx, char *rtm)
{
	char running = smp_test_and_set(rtm);
	if (running) {
		baktmr_callback_busy(idx);
		return;
	}

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
	if (err) {
		error(_("can't backup sav `%s'"), sav->name);
		goto err_out;
	}

	int ret;
	err = proc_wait(&ps, &ret);
	if (err) {
		warn_errno(_("failed finish backup routine for sav `%s'"),
			  sav->name);
		goto err_out;
	}

	if (ret) {
		warn(_("backup routine of sav `%s' eixted with code `%d'"),
		    sav->name, ret);
		goto err_out;
	}

err_out:
	smp_clear(rtm);
}
