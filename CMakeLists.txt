# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(ROOT ${CMAKE_SOURCE_DIR})
set(GENERATED ${ROOT}/include/generated)

include(${ROOT}/scripts/helper.cmake)
file(MAKE_DIRECTORY ${GENERATED})

# ------------------------------- Architecture --------------------------------
if(LINUX)
  include(${ROOT}/scripts/arch-linux.cmake)
endif()

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

test_add_compile_options(CONFIG_USE_ASAN -fsanitize=address)
test_add_link_options(CONFIG_USE_ASAN -fsanitize=address)

if(WIN32 AND CONFIG_USE_ASAN)
  # ASAN library lacks certain C library functions.
  add_compile_options(-U_DLL)
endif()

test_add_compile_options(CONFIG_USE_UBSAN -fsanitize=undefined)
test_add_link_options(CONFIG_USE_UBSAN -fsanitize=undefined)

test_add_compile_options(CONFIG_USE_TSAN -fsanitize=thread)
test_add_link_options(CONFIG_USE_TSAN -fsanitize=thread)

test_add_compile_options(CONFIG_USE_MSAN -fsanitize=memory)
test_add_link_options(CONFIG_USE_MSAN -fsanitize=memory)

# --------------------------------- Compiler ----------------------------------
set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

test_add_compile_options(CONFIG_COMPILER_WARNING_ALL -Wall)

test_add_compile_options(CONFIG_COMPILER_WARNING_EXTRA -Wextra)

kselect(CONFIG_COMPILER_DEBUG)
add_compile_options(-g${CONFIG_COMPILER_DEBUG})

kselect(CONFIG_COMPILER_OPTIMIZE)
add_compile_options(-O${CONFIG_COMPILER_OPTIMIZE})

test_add_compile_options(CONFIG_ENABLE_DEBUGGING -fno-omit-frame-pointer)

test_add_compile_options(CONFIG_USE_POISON -fno-strict-aliasing)

add_compile_options(-Wbad-function-cast
		    -Wformat-security
		    -Wformat-y2k
		    -Winit-self
		    -Wmissing-declarations
		    -Wmissing-prototypes
		    -Wnested-externs
		    -Wno-system-headers
		    -Wold-style-definition
		    -Wpacked
		    -Wstrict-prototypes
		    -Wswitch-enum
		    -Wshadow)

add_compile_options(-fshort-wchar
		    -funsigned-char
		    -fno-common)

# ---------------------------------- Profile ----------------------------------
set(SAVESAVE_NAME savesave)
set(SAVESAVE_VERSION $ENV{SAVESAVE_VERSION})
string(REPLACE -rc . SAVESAVE_VERSION_CMAKE ${SAVESAVE_VERSION})

project(${SAVESAVE_NAME} VERSION ${SAVESAVE_VERSION_CMAKE} LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------- Include ----------------------------------
# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include ${GENERATED}/autoconf.h")
add_compile_options("SHELL:-include ${GENERATED}/appinfo.h")
add_compile_options("SHELL:-include ${ROOT}/include/brtcomcent.h")

include_directories(${ROOT}/include ${ROOT}/include/arch/${ARCH})

# --------------------------------- Manifest ----------------------------------
if(WIN32)
  set(SAVESAVE_VERSION_RC ${SAVESAVE_VERSION_CMAKE})
  string(REPLACE "." ";" rcv_seg ${SAVESAVE_VERSION_CMAKE})

  list(LENGTH rcv_seg rcv_rep)
  string(REPEAT "0." ${rcv_rep} rcv_pad)

  string(PREPEND SAVESAVE_VERSION_RC ${rcv_pad})
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
  if(CONFIG_NO_GUI)
    list(APPEND mainsrc wincon.c)
  else()
    list(APPEND mainsrc wingui.c)
  endif()
endif()
list(TRANSFORM mainsrc PREPEND ${ROOT}/entry/)

if(WIN32)
  list(APPEND mainsrc ${ROOT}/savesave.rc)
  if(NOT CONFIG_NO_GUI)
    list(PREPEND mainsrc WIN32)
  endif()
endif()

add_executable(savesave ${mainsrc})

target_link_libraries(savesave platfree platspec)

# ------------------------------------ Dll ------------------------------------
if(WIN32)
  target_link_libraries(savesave comctl32 shlwapi)
  if(CONFIG_ENABLE_DEBUGGING AND NOT CONFIG_NO_REPORT_HOOK)
    target_link_libraries(savesave dbghelp)
    add_compile_definitions(HAVE_RPHK)
  endif()
endif()

# -------------------------------- Definition ---------------------------------
add_compile_definitions(_GNU_SOURCE)

if(NOT CONFIG_ENABLE_DEBUGGING)
  add_compile_definitions(NDEBUG)
endif()

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
