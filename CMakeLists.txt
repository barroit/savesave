# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

# ----------------------------- build profile ------------------------------
set(ARCH  $ENV{ARCH})
set(BUILD $ENV{BUILD})

# follow the gcc concepts:
#   build is the machine you are building on
#   host  is the machine you are building for
if(BUILD STREQUAL linux)
  set(HOST linux)
else()
  set(HOST windows)
endif()

set(ROOT ${CMAKE_SOURCE_DIR})

# -------------------------- generated directory ---------------------------
set(GENERATED ${ROOT}/include/generated)
file(MAKE_DIRECTORY ${GENERATED})

# -------------------------------- kconfig ---------------------------------
include(${ROOT}/scripts/kconfig.cmake)

# -------------------------------- compiler --------------------------------
if(BUILD STREQUAL linux)
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  # we do not use cpp on linux
else()
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_CXX_NAME})
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS true)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
list(APPEND languages C)

if(HOST STREQUAL windows)
  list(APPEND languages CXX)
endif()

set(SAVESAVE_NAME savesave)
set(SAVESAVE_VERSION $ENV{SAVESAVE_VERSION})

string(REPLACE -rc . __SAVESAVE_VERSION ${SAVESAVE_VERSION})
project(${SAVESAVE_NAME} VERSION ${__SAVESAVE_VERSION} LANGUAGES ${languages})

# -------------------------------- include ---------------------------------
# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include ${GENERATED}/autoconf.h")
add_compile_options("SHELL:-include ${GENERATED}/appinfo.h")
add_compile_options("SHELL:-include ${GENERATED}/apphelp.h")
add_compile_options("SHELL:-include ${ROOT}/include/brtcomcent.h")

include_directories(${ROOT}/include)

# -------------------------------- manifest --------------------------------
if(BUILD STREQUAL windows)
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
if(HOST STREQUAL linux)
  file(GLOB platspec_src LIST_DIRECTORIES false uni/*.c)
  set(mainfile ${ROOT}/entry/savesave.c)
else()
  file(GLOB platspec_src LIST_DIRECTORIES false win/*.cpp win/*.c)
  set(mainfile ${ROOT}/entry/savesave.cpp)
endif()
add_library(platspec OBJECT ${platspec_src})

# -------------------------------- platfree --------------------------------
file(GLOB platfree_src LIST_DIRECTORIES false platfree/*.c)
add_library(platfree OBJECT ${platfree_src})

# -------------------------------- savesave --------------------------------
if(HOST STREQUAL linux)
  add_executable(savesave ${mainfile})
else()
  if(CONFIG_IS_CONSOLE_APP)
    add_executable(savesave ${mainfile} ${ROOT}/savesave.rc)
  else()
    add_executable(savesave WIN32 ${mainfile} ${ROOT}/savesave.rc)
  endif()
endif()
target_link_libraries(savesave platfree platspec)

# ---------------------------------- dll -----------------------------------
if(HOST STREQUAL windows)
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

if(HOST STREQUAL windows)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# ------------------------------ installation ------------------------------
install(TARGETS savesave DESTINATION ".")
install(DIRECTORY ${ROOT}/locale/ DESTINATION locale
	FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)

if(BUILD STREQUAL windows)
  file(GLOB dlls ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dlls} DESTINATION ".")
endif()

# ---------------------------- generated header ----------------------------
include(${ROOT}/scripts/genheader.cmake)
