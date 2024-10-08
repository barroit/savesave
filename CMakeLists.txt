# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(ROOT ${CMAKE_SOURCE_DIR})
set(GENERATED ${ROOT}/include/generated)

include(${ROOT}/scripts/helper.cmake)
file(MAKE_DIRECTORY ${GENERATED})

# ------------------------------- Architecture --------------------------------
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

# ---------------------------------- Kconfig ----------------------------------
execute_process(COMMAND genconfig --header-path ${GENERATED}/autoconf.h
		--config-out ${ROOT}/.config)

file(STRINGS ${ROOT}/.config lines ENCODING UTF-8)

foreach(line ${lines})
  if (line MATCHES "^#")
    continue()
  endif()

  string(REGEX MATCH "^(CONFIG_[^=]+)=(\"?)([^\"]*)(\"?)$" __unused ${line})

  set(${CMAKE_MATCH_1} ${CMAKE_MATCH_3})
  list(APPEND DOTCONFIG_VARIABLES "${CMAKE_MATCH_1}")
endforeach()

# --------------------------------- Sanitizer ---------------------------------
if(CONFIG_USE_ASAN AND (CONFIG_USE_TSAN OR CONFIG_USE_MSAN))
  message(FATAL_ERROR "AddressSanitizer is "
	  "incompatible with ThreadSanitizer and MemorySanitizer.")
endif()

if(CONFIG_USE_UBSAN AND CONFIG_USE_MSAN)
  message(FATAL_ERROR "AddressSanitizer is "
	  "incompatible with MemorySanitizer.")
endif()

if(CONFIG_USE_TSAN AND (CONFIG_USE_ASAN OR CONFIG_USE_MSAN))
  message(FATAL_ERROR "ThreadSanitizer is "
	  "incompatible with AddressSanitizer and MemorySanitizer.")
endif()

if(CONFIG_USE_MSAN AND (CONFIG_USE_ASAN OR
			CONFIG_USE_UBSAN OR CONFIG_USE_TSAN))
  message(FATAL_ERROR "MemorySanitizer is "
	  "incompatible with all other sanitizers.")
endif()

test_add_complie_options(CONFIG_USE_ASAN -fsanitize=address)
test_add_link_options(CONFIG_USE_ASAN -fsanitize=address)

test_add_complie_options(CONFIG_USE_UBSAN -fsanitize=undefined)
test_add_link_options(CONFIG_USE_UBSAN -fsanitize=undefined)

test_add_complie_options(CONFIG_USE_TSAN -fsanitize=thread)
test_add_link_options(CONFIG_USE_TSAN -fsanitize=thread)

test_add_complie_options(CONFIG_USE_MSAN -fsanitize=memory)
test_add_link_options(CONFIG_USE_MSAN -fsanitize=memory)

# --------------------------------- Compiler ----------------------------------
set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

test_add_complie_options(CONFIG_COMPILER_WARNING_ALL -Wall)

test_add_complie_options(CONFIG_COMPILER_WARNING_EXTRA -Wextra)

kselect(CONFIG_COMPILER_DEBUG)
add_compile_options(-g${CONFIG_COMPILER_DEBUG})

kselect(CONFIG_COMPILER_OPTIMIZE)
add_compile_options(-O${CONFIG_COMPILER_OPTIMIZE})

# ---------------------------------- Profile ----------------------------------
set(SAVESAVE_NAME savesave)
set(SAVESAVE_VERSION $ENV{SAVESAVE_VERSION})
string(REPLACE -rc . SAVESAVE_VERSION_CMAKE ${SAVESAVE_VERSION})

project(${SAVESAVE_NAME} VERSION ${SAVESAVE_VERSION_CMAKE} LANGUAGES C)

# ---------------------------------- Include ----------------------------------
# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include ${GENERATED}/autoconf.h")
add_compile_options("SHELL:-include ${GENERATED}/appinfo.h")
add_compile_options("SHELL:-include ${ROOT}/include/brtcomcent.h")

include_directories(${ROOT}/include ${ROOT}/include/arch/${ARCH})

# --------------------------------- Manifest ----------------------------------
if(WIN32)
  configure_file(${ROOT}/savesave.manifest.in ${ROOT}/savesave.manifest)
endif()

# ---------------------------------- Gettext ----------------------------------
find_package(Intl)
if(NOT CONFIG_DISALBE_GETTEXT AND Intl_FOUND)
  link_libraries(Intl::Intl)
  add_compile_definitions(HAVE_INTL)

  if(NOT Intl_VERSION)
    find_package(Gettext)
    if(GETTEXT_FOUND)
      set(GETTEXT_VERSION ${GETTEXT_VERSION_STRING})
    else()
      set(GETTEXT_VERSION "unknown")
    endif()
  else()
    set(GETTEXT_VERSION ${Intl_VERSION})
  endif()
endif()

# ----------------------------------- Zstd ------------------------------------
find_package(zstd CONFIG REQUIRED)
if(CONFIG_LINK_LIBZSTD_STATIC)
  link_libraries(zstd::libzstd_static)
else()
  link_libraries(zstd::libzstd_shared)
endif()

# --------------------------------- Platspec ----------------------------------
if(UNIX)
  list(APPEND pathspec unix/*.c)
elseif(WIN32)
  list(APPEND pathspec win32/*.c)
endif()

if(LINUX)
  list(APPEND pathspec linux/*.c)
endif()

list(TRANSFORM pathspec PREPEND platspec/)
file(GLOB source LIST_DIRECTORIES false ${pathspec})

add_library(platspec OBJECT ${source})

# --------------------------------- Platfree ----------------------------------
file(GLOB source LIST_DIRECTORIES false platfree/*.c commands/*.c)
add_library(platfree OBJECT ${source})

# --------------------------------- Savesave ----------------------------------
list(APPEND mainsrc savesave.c)
if(UNIX)
  list(APPEND mainsrc uni.c)
elseif(WIN32)
  if(CONFIG_IS_CONSOLE_APP)
    list(APPEND mainsrc wincon.c)
  else()
    list(APPEND mainsrc wingui.c)
  endif()
endif()
list(TRANSFORM mainsrc PREPEND ${ROOT}/entry/)

if(WIN32)
  list(APPEND mainsrc ${ROOT}/savesave.rc)
  if(NOT CONFIG_IS_CONSOLE_APP)
    list(PREPEND mainsrc WIN32)
  endif()
endif()

add_executable(savesave ${mainsrc})

target_link_libraries(savesave platfree platspec)

# ------------------------------------ Dll ------------------------------------
if(WIN32)
  target_link_libraries(savesave ws2_32 comctl32 shlwapi)
  if(NOT CONFIG_SOURCE_NDEBUG)
    target_link_libraries(savesave dbghelp)
  endif()
endif()

# -------------------------------- Definition ---------------------------------
add_compile_definitions(_GNU_SOURCE)

test_add_compile_definitions(CONFIG_SOURCE_NDEBUG NDEBUG)

test_add_compile_definitions(WIN32
			     NTDDI_WIN7
			     WINVER=_WIN32_WINNT_WIN7
			     _WIN32_WINNT=_WIN32_WINNT_WIN7
			     _CRT_SECURE_NO_WARNINGS)

# ------------------------------- Installation --------------------------------
install(TARGETS savesave DESTINATION ".")
install(DIRECTORY ${ROOT}/locale/ DESTINATION locale
	FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)

if(WIN32)
  file(GLOB dlls ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dlls} DESTINATION ".")
endif()

# ----------------------------- Generated Headers -----------------------------
file(GLOB files ${ROOT}/include/*.in)

foreach(conf ${files})
  cmake_path(GET conf STEM LAST_ONLY name)
  configure_file(${conf} ${GENERATED}/${name})
endforeach()
