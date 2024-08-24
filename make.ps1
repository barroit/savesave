# SPDX-License-Identifier: GPL-3.0-only

param (
	[ValidateSet('build', 'distclean', 'menuconfig')]
	[string]$command = 'build'
)

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

$vcpgk_tc  = "$env:VCPKG_TOOLCHAIN".Replace('\','/')
$abs_root  = "$PSScriptRoot".Replace('\','/')
$abs_root  = $abs_root.Substring(0, $abs_root.Length - 1)
$abs_build = "$abs_root/build"

$env:BUILD = 'windows'
$env:ARCH  = $env:PROCESSOR_ARCHITECTURE
$env:SAVESAVE_VERSION = cat version

if ($command -eq 'distclean') {
	rm -Force "$abs_root/include/generated/*"
	rm -Recurse -Force "$abs_build/*"
	rm -Force "$abs_root/.config*"
	exit 0
} elseif ($command -eq 'menuconfig') {
	$env:MENUCONFIG_STYLE = 'aquatic'
	menuconfig
	exit 0
}

cmake -G Ninja "$abs_root" -B "$abs_build" -DCMAKE_TOOLCHAIN_FILE="$vcpgk_tc"
if (! $?) {
	exit 1
}

cmake --build "$abs_build" --parallel
if (! $?) {
	exit 1
}

& "$env:MT" -nologo -manifest "$abs_root/savesave.manifest" `
	    -outputresource:"$abs_build/savesave.exe";#1
