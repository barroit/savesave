# SPDX-License-Identifier: GPL-3.0-or-later
# We use the first item in the list as real architecture name
# Also include/arch
set(ARCH_X86 "x86 x86_64 amd64 x64")
set(ARCH_RISCV "riscv riscv riscv64")
set(ARCH_ARM64 "arm64 arm64 aarch64")

string(TOLOWER $ENV{ARCH} ARCH)
foreach(AHCHLIST IN LISTS ARCH_X86 ARCH_RISCV ARCH_ARM64)
  string(REGEX MATCH ${ARCH} FINDARCH ${AHCHLIST})
  if(NOT FINDARCH)
    continue()
  endif()

  string(REPLACE " " ";" AHCHLIST ${AHCHLIST})
  list(GET AHCHLIST 0 ARCH)
  set(ARCH_FOUND 1)
endforeach()

if(NOT ARCH_FOUND)
  set(ARCH generic)
endif()

file(CREATE_LINK ${ROOT}/arch/${ARCH} ${ROOT}/include/arch SYMBOLIC)
