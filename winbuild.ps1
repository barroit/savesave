# SPDX-License-Identifier: GPL-3.0-only

if (! $env:MT) {
	Write-Error "`$MT is undefined, see INSTALL.win for help" `
		    -Category ObjectNotFound
	exit 1
}

if (! $env:VCPKG_TOOLCHAIN) {
	Write-Error "`$VCPKG_TOOLCHAIN is undefined, see INSTALL.win for help" `
		    -Category ObjectNotFound
	exit 1
}

if (! (Test-Path ./Kconfig)) {
	Write-Error "Can't find Kconfig, make sure you are in root directory" `
		    -Category ObjectNotFound
	exit 1
}

$vcpgk_tc  = "$env:VCPKG_TOOLCHAIN".Replace('\','/')
$abs_root  = "$pwd".Replace('\','/')
$abs_root  = $abs_root.Substring(0, $abs_root.Length - 1)
$abs_build = "$abs_root/build"
$abs_gen   = "$abs_root/include/generated"

$env:ROOT = $abs_root

$env:DOTCONFIG = "$abs_root/.config"
$env:AUTOCONF  = "$abs_gen/autoconf.h"

$env:BUILD = "windows"
$env:ARCH = $env:PROCESSOR_ARCHITECTURE

mkdir -Force $abs_gen > $null
genconfig --header-path $env:AUTOCONF --config-out .config

cmake -G Ninja $abs_root -B $abs_build -DCMAKE_TOOLCHAIN_FILE="$vcpgk_tc"
if (! $?) {
	exit 1
}

cmake --build $abs_build --parallel
if (! $?) {
	exit 1
}

& "$env:MT" -nologo -manifest $abs_root/savesave.manifest `
	    -outputresource:$abs_build/savesave.exe;#1
