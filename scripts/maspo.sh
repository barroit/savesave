#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-only

if [[ ! -f .savesave.example ]]; then
	>&2 echo not in source tree
	exit 1
fi

region=('ja_JP' 'zh_CN')

case $1 in
'update')
	xgettext -k_ -kN_ -k -i --omit-header --from-code=UTF-8		\
		 -o locale/generated.pot				\
		 uni/*.c	win/*.c		win/*.cpp		\
		 platfree/*.c						\
		 entry/*.c	entry/*.cpp				\
		 include/*.h	include/barroit/*.h	include/win/*.hpp

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

