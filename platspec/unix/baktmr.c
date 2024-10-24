// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktmr.h"
#include "list.h"
#include "dotsav.h"
#include "alloc.h"
#include "termas.h"
#include "barrier.h"

struct baktmr {
	timer_t *tmr;		/* timer ids */
	u8 *rtm;		/* running task map */
};

/* overload counter */
struct bakolc {
	uint head;
	uint tail;
	uint mask;
	timestamp_t arr[1 << 3];

	int killed;
	mtx_t lock;
};

static struct baktmr btmr;
static struct bakolc bolc = {
	.mask = sizeof_array(bolc.arr) - 1,
};

static void tmr_callback_busy(uint i)
{
	timestamp_t new = timestamp();
	timestamp_t old;

	mtx_lock(&bolc.lock);
	if (bolc.killed) {
		mtx_unlock(&bolc.lock);
		return;
	}

	uint tail = bolc.tail;
	uint head = bolc.head;

	if (tail == head)
		goto push_ts;

	while (head != tail) {
		old = bolc.arr[head & bolc.mask];
		if (new - old <= BAKTMR_OVERLOAD_WINDOW)
			break;

		head += 1;
	}

	bolc.head = head;
	if (head == tail)
		goto push_ts;

	if (tail + 1 - head > BAKTMR_MAX_OVERLOAD) {
		bolc.killed = 1;
		mtx_unlock(&bolc.lock);

		error(_("too many backup requests were dropped, "
		      "savesave can't keep up anymore"));
		kill(0, SIGTERM);
	}

push_ts:
	bolc.arr[tail & bolc.mask] = new;
	tail += 1;

	bolc.tail = tail;
	mtx_unlock(&bolc.lock);

	warn(_("backup request of sav `%s' was dropped"),
	     dotsav_array[i].name);
}

static void tmr_callback(sigval_t sv)
{
	uint i = sv.sival_int;

	u8 running = smp_load_acquire(&btmr.rtm[i]);
	if (running) {
		tmr_callback_busy(i);
		return;
	}

	smp_store_release(&btmr.rtm[i], 1);

	puts(dotsav_array[i].name);
	sleep(2);

	smp_store_release(&btmr.rtm[i], 0);
}

void baktmr_arm(void)
{
	mtx_init(&bolc.lock, mtx_plain);

	btmr.tmr = xcalloc(sizeof(*btmr.tmr), dotsav_size);
	btmr.rtm = xcalloc(sizeof(*btmr.rtm), dotsav_size);

	int err;
	size_t i;
	struct itimerspec ts = { 0 };
	struct sigevent sev = {
		.sigev_notify           = SIGEV_THREAD,
		.sigev_notify_function  = tmr_callback,
	};

	for_each_idx(i, dotsav_size) {
		sev.sigev_value.sival_int = i;
		err = timer_create(CLOCK_REALTIME, &sev, &btmr.tmr[i]);
		if (err)
			goto err_creat_tmr;
	}

	for_each_idx(i, dotsav_size) {
		ts.it_value.tv_sec = dotsav_array[i].period;
		ts.it_interval.tv_sec = dotsav_array[i].period;

		err = timer_settime(btmr.tmr[i], 0, &ts, NULL);
		BUG_ON(err);
	}

	return;

err_creat_tmr:
	die_errno(_("failed to create backup timer for sav `%s'"),
		  dotsav_array[i].name);
}

void baktmr_disarm(void)
{
	size_t i;
	struct itimerspec ts = { 0 };

	for_each_idx(i, dotsav_size) {
		int err = timer_settime(btmr.tmr[i], 0, &ts, NULL);
		BUG_ON(err);
	}
}
