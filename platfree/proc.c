// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "proc.h"
#include "strbuf.h"
#include "robio.h"
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

pid_t check_unique_process(char **name, int abort)
{
	pid_t pid;
	size_t i;
	struct strbuf path = STRBUF_INIT;
	const char *piddir[] = DATA_DIR_LIST_INIT;

	for_each_idx(i, sizeof_array(piddir)) {
		if (piddir[i] == NULL)
			continue;

		strbuf_concat_path(&path, piddir[i], PROCID_NAME);

		int err = readpid(path.str, &pid);
		if (err)
			goto next;

		if (process_is_alive(pid)) {
			if (abort)
				goto pid_not_unique;
			*name = path.str;
			return pid;
		}

		unlink(path.str);
next:
		strbuf_reset(&path);
	}

	strbuf_destroy(&path);
	return max_int_valueof(pid);

pid_not_unique:
	die(_("there is already a running savesave (PID %d)"), pid);
}

void push_process_id(void)
{
	const char *name = pid_path();
	pid_t pid = getpid();
	char buf[STRPID_MAX];

	int snr = snprintf(buf, sizeof(buf), "%d", pid);
	BUG_ON(snr < 0);

	int fd = flexcreat(name);
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
	const char *name = pid_path();
	unlink(name);
}
