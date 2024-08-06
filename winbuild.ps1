# SPDX-License-Identifier: GPL-3.0

if (! $env:MT_PATH) {
	$message = "`$MT_PATH is undefined, refer to the INSTALL.win for " `
		    + "configuration instructions"
	Write-Error $message -Category ObjectNotFound
	exit 1
}

if (! (Test-Path ./Kconfig)) {
	Write-Error "Can't find Kconfig, make sure you are in root directory" `
		    -Category ObjectNotFound
	exit 1
}

$abs_root  = "$pwd".Replace('\','/')
$abs_root  = $abs_root.Substring(0, $abs_root.Length - 1)
$abs_build = "$abs_root/build"
$abs_gen   = "$abs_root/include/generated"

$env:ROOT = $abs_root

$env:DOTCONFIG = "$abs_root/.config"
$env:AUTOCONF  = "$abs_gen/autoconf.h"

$env:HOST = "windows"
$env:ARCH = $env:PROCESSOR_ARCHITECTURE

mkdir -Force $abs_gen > $null
genconfig --header-path $env:AUTOCONF --config-out .config

cmake -G Ninja $abs_root -B $abs_build
cmake --build $abs_build --parallel

& "$env:MT_PATH" -nologo -manifest $abs_root/savesave.manifest `
		 -outputresource:$abs_build/savesave.exe;#1
