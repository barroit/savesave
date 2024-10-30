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
#include "iter.h"
#include "termas.h"

static pid_t read_pid(const char *name)
{
	int fd;
	char pidstr[STRPID_MAX];

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return max_value(pid_t);

	int nr = robread(fd, pidstr, sizeof(pidstr));
	if (nr == sizeof(pidstr)) /* truncated? */
		goto err_not_pid;
	else if (nr == 0) /* dangling file? */
		goto err_not_pid;

	pidstr[nr] = 0;
	llong pid;
	int ret = str2llong(pidstr, -1, &pid, 0, max_value(pid_t));
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

pid_t pid_retrieve(void)
{
	size_t i;
	pid_t pid = max_value(pid);
	struct strbuf path = STRBUF_INIT;
	const char *piddir[] = DATA_DIR_LIST_INIT;

	for_each_idx(i, sizeof_array(piddir)) {
		if (piddir[i] == NULL)
			continue;

		strbuf_concat_pathname(&path, piddir[i], PROCID_NAME);

		pid = read_pid(path.str);
		if (pid == max_value(pid))
			goto next;

		if (pid_is_alive(pid))
			goto out;

		/*
		 * We have pid file, but the process associated with
		 * that pid is dead.
		 */
		unlink(path.str);

next:
		strbuf_reset(&path);
	}

out:
	strbuf_destroy(&path);
	return pid;
}

void proc_assert_unique(void)
{
	pid_t pid = pid_retrieve();

	if (pid == max_value(pid))
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

int proc_rd_io(const char *name, flag_t flags)
{
	BUG_ON(!flags);

	int fd = flexcreat_nt(name);
	if (fd == -1)
		return -1;

	int ret = 0;
	if (flags & PROC_RD_STDIN) {
		ret = dup2(fd, STDIN_FILENO);
		if (ret == -1) {
			ret = PERR_RD_STDIN;
			goto err_out;
		}
	}

	if (flags & PROC_RD_STDOUT) {
		ret = dup2(fd, STDOUT_FILENO);
		if (ret == -1) {
			ret = PERR_RD_STDOUT;
			goto err_out;
		}
	}

	if (flags & PROC_RD_STDERR) {
		ret = dup2(fd, STDERR_FILENO);
		if (ret == -1) {
			ret = PERR_RD_STDERR;
			goto err_out;
		}
	}

err_out:
	close(fd);
	return ret >= 0 ? 0 : ret;
}
