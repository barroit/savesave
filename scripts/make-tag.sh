#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later

if [[ ! -f .savesave.example ]]; then
	>&2 echo 'error: not in source tree'
	exit 1
fi

if [[ ! $1 ]]; then
	>&2 echo 'error: missing version-spec'
	exit 1
fi

echo $1 > ./version

git add ./version

git commit -m "Savesave $1"

git tag -sm "Savesave $1" "v$1"
