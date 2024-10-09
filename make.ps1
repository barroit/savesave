# SPDX-License-Identifier: GPL-3.0-only

param (
	[ValidateSet('build', 'distclean', 'menuconfig', `
		     'install', 'uninstall', 'mcheck')]
	[string]$command = 'build'
)

if (! (Test-Path .savesave.example)) {
	Write-Error "not on source tree"
	exit 1
}

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

$install_dest = "$env:LOCALAPPDATA".Replace('\','/') + "/Programs/savesave"

$env:WIN32 = 'y'
$env:BUILD = 'windows'
$env:ARCH  = $env:PROCESSOR_ARCHITECTURE
$env:SAVESAVE_VERSION = cat version

if ($command -eq 'distclean') {
	rm -Force -ErrorAction SilentlyContinue "$abs_root/include/generated/*"
	git ls-files --directory -o 'build' | rm -Recurse -Force
	rm -Force -ErrorAction SilentlyContinue "$abs_root/.config*"
	rm -Force -ErrorAction SilentlyContinue "$abs_root/savesave.manifest"
	exit 0
} elseif ($command -eq 'menuconfig') {
	$env:MENUCONFIG_STYLE = 'aquatic'
	menuconfig
	exit 0
} elseif ($command -eq 'install') {
	cmake --install "$abs_build" --config Release
	exit 0
} elseif ($command -eq 'uninstall') {
	rm -Recurse -Force -ErrorAction SilentlyContinue "$install_dest"
	exit 0
} elseif ($command -eq 'mcheck') {
	drmemory -check_uninit_blocklist "KERNELBASE.dll,ntdll.dll" `
		 -suppress .suppress .\build\savesave.exe $args
	exit 0
}

cmake -G Ninja "$abs_root" -B "$abs_build" `
      -DCMAKE_INSTALL_PREFIX="$install_dest" `
      -DCMAKE_TOOLCHAIN_FILE="$vcpgk_tc" `
      -DCMAKE_BUILD_TYPE=Debug
if (! $?) {
	exit 1
}

cmake --build "$abs_build" --parallel
if (! $?) {
	exit 1
}

& "$env:MT" -nologo -manifest "$abs_root/savesave.manifest" `
	    -outputresource:"$abs_build/savesave.exe";#1
