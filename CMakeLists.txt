# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(ARCH $ENV{ARCH})
set(ROOT ${CMAKE_SOURCE_DIR})

# -------------------------- generated directory ---------------------------
set(GENERATED ${ROOT}/include/generated)
file(MAKE_DIRECTORY ${GENERATED})

# -------------------------------- kconfig ---------------------------------
include(${ROOT}/scripts/kconfig.cmake)

# -------------------------------- compiler --------------------------------
set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CONFIG_COMPILER_WARNING_ALL)
  add_compile_options(-Wall)
endif()

if(CONFIG_COMPILER_WARNING_EXTRA)
  add_compile_options(-Wextra)
endif()

set_choiced_value(CONFIG_COMPILER_DEBUG)
add_compile_options(-g${CONFIG_COMPILER_DEBUG})

set_choiced_value(CONFIG_COMPILER_OPTIMIZE)
add_compile_options(-O${CONFIG_COMPILER_OPTIMIZE})

# ---------------------------- project profile -----------------------------
set(SAVESAVE_NAME savesave)
set(SAVESAVE_VERSION $ENV{SAVESAVE_VERSION})
string(REPLACE -rc . SAVESAVE_VERSION_CMAKE ${SAVESAVE_VERSION})

project(${SAVESAVE_NAME} VERSION ${SAVESAVE_VERSION_CMAKE} LANGUAGES C)

# -------------------------------- include ---------------------------------
# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include ${GENERATED}/autoconf.h")
add_compile_options("SHELL:-include ${GENERATED}/appinfo.h")
add_compile_options("SHELL:-include ${ROOT}/include/brtcomcent.h")

include_directories(${ROOT}/include)

# -------------------------------- manifest --------------------------------
if(WIN32)
  configure_file(${ROOT}/savesave.manifest.in ${ROOT}/savesave.manifest)
endif()

# -------------------------------- gettext ---------------------------------
find_package(Intl)
if(NOT CONFIG_DISALBE_GETTEXT AND Intl_FOUND)
  link_libraries(Intl::Intl)
  add_compile_definitions(HAVE_INTL)

  if(NOT Intl_VERSION)
    find_package(Gettext)
    if (GETTEXT_FOUND)
      set(GETTEXT_VERSION ${GETTEXT_VERSION_STRING})
    else()
      set(GETTEXT_VERSION "unknown")
    endif()
  else()
    set(GETTEXT_VERSION ${Intl_VERSION})
  endif()
endif()

# ---------------------------------- zstd ----------------------------------
find_package(zstd CONFIG REQUIRED)
if(CONFIG_LINK_LIBZSTD_STATIC)
  link_libraries(zstd::libzstd_static)
else()
  link_libraries(zstd::libzstd_shared)
endif()

# -------------------------------- platspec --------------------------------
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

# -------------------------------- platfree --------------------------------
file(GLOB source LIST_DIRECTORIES false platfree/*.c commands/*.c)
add_library(platfree OBJECT ${source})

# -------------------------------- savesave --------------------------------
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

# ---------------------------------- dll -----------------------------------
if(WIN32)
  target_link_libraries(savesave ws2_32 comctl32 shlwapi)
  if (NOT CONFIG_SOURCE_NDEBUG)
    target_link_libraries(savesave dbghelp)
  endif()
endif()

# ------------------------------- definition -------------------------------
add_compile_definitions(_GNU_SOURCE)

if(CONFIG_SOURCE_NDEBUG)
  add_compile_definitions(NDEBUG)
endif()

if(WIN32)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7
			  _CRT_SECURE_NO_WARNINGS)
endif()

# ------------------------------ installation ------------------------------
install(TARGETS savesave DESTINATION ".")
install(DIRECTORY ${ROOT}/locale/ DESTINATION locale
	FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)

if(WIN32)
  file(GLOB dlls ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dlls} DESTINATION ".")
endif()

# ---------------------------- generated header ----------------------------
include(${ROOT}/scripts/genheader.cmake)
