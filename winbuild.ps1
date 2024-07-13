if (! (Test-Path ./Kconfig)) {
	Write-Error "Can't find Kconfig, make sure you are in root directory" `
		    -Category ObjectNotFound
}

$abs_root  = (Get-Item ./Kconfig).Directory.ToString().Replace('\','/')
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
cmake --build $abs_build
