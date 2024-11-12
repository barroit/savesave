#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later

if [[ ! -f .savesave.example ]]; then
	>&2 echo not in source tree
	exit 1
fi

stack=$(( ${1:-5} - 1 ))

mkdir -p sandbox/backup

for i in $(seq 0 $stack); do
	touch sandbox/backup/overworld.$i
done
