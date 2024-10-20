/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Generated by scripts/gen-command.sh
 */

#ifndef BRTGEN_COMMAND_H
#define BRTGEN_COMMAND_H

#define USEDOTSAV   /* use dotsav */

#define CMD_USEDOTSAV   (1 << 1)

#define CMDDESCRIP(usage)

int cmd_backup(int argc, const char **argv);
int cmd_copy(int argc, const char **argv);
int cmd_help(int argc, const char **argv);
int cmd_query(int argc, const char **argv);
int cmd_sizeof(int argc, const char **argv);
int cmd_version(int argc, const char **argv);

#define APOPT_MAINCOMMAND(v) \
	APOPT_SUBCMD("backup", (v), N_("Start backup task in background"), cmd_backup, CMD_USEDOTSAV), \
	APOPT_SUBCMD("copy", (v), N_("Copy a file"), cmd_copy, 0), \
	APOPT_SUBCMD("help", (v), N_("Display help information about Savesave"), cmd_help, 0), \
	APOPT_SUBCMD("query", (v), N_("Query default information of Savesave"), cmd_query, 0), \
	APOPT_SUBCMD("sizeof", (v), N_("Calculate file size of given path"), cmd_sizeof, 0), \
	APOPT_SUBCMD("version", (v), N_("Display version information about Savesave"), cmd_version, 0)

#endif /* BRTGEN_COMMAND_H */
