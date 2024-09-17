/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Generated by scripts/gencmdh.sh
 * This file should only be included in arugpar.h.
 */

#ifndef GEN_COMMAND_H
#define GEN_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

int cmd_help(int argc, const char **argv);
int cmd_version(int argc, const char **argv);

#define APOPT_MAIN_COMMAND_INIT(v) { \
	APOPT_SUBCOMMAND("help", (v), cmd_help), \
	APOPT_SUBCOMMAND("version", (v), cmd_version), \
	APOPT_END(), \
}

#ifdef __cplusplus
}
#endif

#endif /* GEN_COMMAND_H */
