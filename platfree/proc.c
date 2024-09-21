// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "strbuf.h"
#include "robio.h"
#include "debug.h"
#include "path.h"
#include "list.h"
#include "termas.h"

static int readpid(const char *name, pid_t *pid)
{
	int fd;
	char pidstr[STRPID_MAX];

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return -1;

	int nr = robread(fd, pidstr, sizeof(pidstr));
	if (nr == sizeof(pidstr)) /* truncated? */
		goto err_not_pid;
	else if (nr == 0) /* dangling file? */
		goto err_not_pid;

	pidstr[nr] = 0;
	llong __pid;
	int ret = str2llong(pidstr, -1, &__pid, 0, max_int_valueof(*pid));
	if (ret)
		goto err_not_pid;

	*pid = __pid;
	close(fd);
	return ret;

err_not_pid:
	/* this error message is so fucking long :( */
	die(_("PID found in file `%s' is malformed; ensure savesave is completely terminated and manually remove this file."),
	    name);
}

void check_unique_process(void)
{
	pid_t pid;
	int err;
	struct strbuf path = STRBUF_INIT;
	const char *piddir[] = PROCID_DIRLIST_INIT;
	size_t i;

	for_each_idx(i, sizeof_array(piddir)) {
		if (piddir[i] == NULL)
			continue;

		strbuf_printf(&path, "%s/%s", piddir[i], PROCID_NAME);
		err = readpid(path.str, &pid);
		if (err)
			goto next;

		if (PLATSPECOF(is_process_alive)(pid))
			goto err_not_unique;

		unlink(path.str);
next:
		strbuf_reset(&path);
	}

	strbuf_destroy(&path);
	return;

err_not_unique:
	die(_("there is already a running savesave (PID %d)"), pid);
}

void push_process_id(void)
{
	const char *name = get_procid_filename();
	pid_t pid = getpid();
	char buf[STRPID_MAX];

	int snr = snprintf(buf, sizeof(buf), "%d", pid);
	BUG_ON(snr < 0);

	int fd = creat(name, 0664);
	if (fd == -1)
		goto err_init_pidfile;

	ssize_t nr = robwrite(fd, buf, snr);
	if (nr < 0)
		goto err_init_pidfile;

	close(fd);
	return;

err_init_pidfile:
	warn_errno(_("failed to init pid file `%s', unique process detection disabled"),
		   name);
	warn(_("ensure only one savesave process runs at a time, behavior otherwise is undefined"));
}

void pop_process_id(void)
{
	const char *name = get_procid_filename();
	unlink(name);
}
