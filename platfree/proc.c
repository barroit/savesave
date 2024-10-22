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

static pid_t read_pid(const char *name)
{
	int fd;
	char pidstr[STRPID_MAX];

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return max_int_valueof(pid_t);

	int nr = robread(fd, pidstr, sizeof(pidstr));
	if (nr == sizeof(pidstr)) /* truncated? */
		goto err_not_pid;
	else if (nr == 0) /* dangling file? */
		goto err_not_pid;

	pidstr[nr] = 0;
	llong pid;
	int ret = str2llong(pidstr, -1, &pid, 0, max_int_valueof(pid_t));
	if (ret)
		goto err_not_pid;

	close(fd);
	return pid;

err_not_pid:
	/* this error message is so fucking long :( */
	die(_("pid found in file `%s' is malformed; ensure savesave "
	    "is completely terminated and manually remove this file."),
	    name);
}

pid_t savesave_pid(void)
{
	size_t i;
	pid_t pid = max_int_valueof(pid);
	struct strbuf path = STRBUF_INIT;
	const char *piddir[] = DATA_DIR_LIST_INIT;

	for_each_idx(i, sizeof_array(piddir)) {
		if (piddir[i] == NULL)
			continue;

		strbuf_concat_path(&path, piddir[i], PROCID_NAME);

		pid = read_pid(path.str);
		if (pid == max_int_valueof(pid))
			goto next;

		if (proc_is_alive(pid))
			return pid;

		/*
		 * We have pid file, but the process associated with
		 * that pid is dead.
		 */
		unlink(path.str);

next:
		strbuf_reset(&path);
	}

	strbuf_destroy(&path);
	return pid;
}

void assert_unic_proc(void)
{
	pid_t pid = savesave_pid();

	if (pid == max_int_valueof(pid))
		return;

	die(_("only one process allowed (running process `%d')"), pid);
}

void pid_dump(void)
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

void pid_erase(void)
{
	const char *name = pid_path();
	unlink(name);
}
