#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-only

if [[ ! -f .savesave.example ]]; then
	>&2 echo not in source tree
	exit 1
fi

region=('ja_JP' 'zh_CN')

case $1 in
'update')
	xgettext --from-code=UTF-8 --omit-header	\
		 -k_ -kN_ -k -i -o locale/generated.pot	\
		 uni/*.c	win/*.c			\
		 platfree/*.c				\
		 entry/*.c				\
		 include/*.h	include/barroit/*.h

	for r in ${region[@]}; do
		msgmerge -i -U locale/$r.po locale/generated.pot
	done
	;;
'build' | '')
	for r in ${region[@]}; do
		msgfmt -o locale/$r/LC_MESSAGES/savesave.mo locale/$r.po
	done
	;;
*)
	>&2 echo "unknown operation ‘$1’"
	exit 1
esac

