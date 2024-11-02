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

#define CMDDESCRIP(usage)

int cmd_backup(int argc, const char **argv);
int cmd_copy(int argc, const char **argv);
int cmd_help(int argc, const char **argv);
int cmd_query(int argc, const char **argv);
int cmd_sizeof(int argc, const char **argv);
int cmd_version(int argc, const char **argv);
int cmd_main(int argc, const char **argv);

/*
 * Do not use these cm_*_path variables directly. Instead, use corresponding
 * *_path() wrapper functions declared in path.h.
 */
extern const char *cm_dotsav_path;
extern const char *cm_output_path;

extern int cm_has_output;
extern int cm_no_detach;

#define CM_OUTPUT_UNSET (void *)-39
#define cm_io_need_update (!cm_has_output || cm_output_path != CM_OUTPUT_UNSET)

#define APOPT_MAINCOMMAND(v) \
	APOPT_SUBCMD("backup", (v), N_("Start backup task in background"), cmd_backup, 0), \
	APOPT_SUBCMD("copy", (v), N_("Copy a file"), cmd_copy, 0), \
	APOPT_SUBCMD("help", (v), N_("Display help information about Savesave"), cmd_help, 0), \
	APOPT_SUBCMD("query", (v), N_("Query default information of Savesave"), cmd_query, 0), \
	APOPT_SUBCMD("sizeof", (v), N_("Calculate file size of given path"), cmd_sizeof, 0), \
	APOPT_SUBCMD("version", (v), N_("Display version information about Savesave"), cmd_version, 0)

#endif /* BRTGEN_COMMAND_H */
