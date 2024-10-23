#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-only

if [[ ! -f .savesave.example ]]; then
	>&2 echo not in source tree
	exit 1
fi

src=$(echo commands/*.c)
dest=include/command.h

declare=$(awk '
/^$/ {
	buf = ""
	next
}

/^int cmd_[a-zA-Z_]+\(/ {
	getline usage
	if (!usage)
		exit 1

	split($0, res, /cmd_|[()]/)
	cmd = res[2]

	if (buf)
		print cmd "†" usage "†" buf
	else
		print cmd "†" usage

	nextfile
}

{
	if (buf)
		buf = buf " | "
	buf = buf "CMD_" $0
}' $src)

cat <<EOF > $dest
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

EOF

awk -F'†' '{
	printf "int cmd_%s(int argc, const char **argv);\n", $1
}' <<< $declare >> $dest

cat <<EOF >> $dest
int cmd_main(int argc, const char **argv);

/*
 * Do not use these cm_*_path variables directly. Instead, use corresponding
 * *_path() wrapper functions declared in path.h.
 */
extern const char *cm_dotsav_path;
extern const char *cm_output_path;

extern int cm_has_output;

#define APOPT_MAINCOMMAND(v) \\
EOF

awk -F'†' '
{
	printf "\tAPOPT_SUBCMD(\"%s\", (v), N_%s, cmd_%s, ",
	       $1, substr($2, 11), $1

	if ($3)
		printf "%s)", $3
	else
		printf "0)"

	if (NR != n)
		print ", \\"
	else
		print ""

}' n=$(wc -l <<< $declare) <<< $declare >> $dest

cat <<EOF >> $dest

#endif /* BRTGEN_COMMAND_H */
EOF
