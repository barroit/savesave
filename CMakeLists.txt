# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(BUILD $ENV{BUILD})
set(ARCH  $ENV{ARCH})

set(SAVESAVE_NAME savesave)
set(SAVESAVE_VERSION $ENV{SAVESAVE_VERSION})

# follow the gcc concepts:
#   build is the machine you are building on
#   host  is the machine you are building for
if(BUILD STREQUAL linux)
  set(HOST linux)
else()
  set(HOST windows)
endif()

set(ROOT ${CMAKE_SOURCE_DIR})

set(GENERATED ${ROOT}/include/generated)
file(MAKE_DIRECTORY ${GENERATED})

include(${ROOT}/scripts/kconfig.cmake)
include(${ROOT}/scripts/genheader.cmake)

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

# By specifying a SHELL and quoting our options, we can disable Option
# De-duplication
add_compile_options("SHELL:-include ${GENERATED}/autoconf.h")
add_compile_options("SHELL:-include ${GENERATED}/appinfo.h")
add_compile_options("SHELL:-include ${GENERATED}/apphelp.h")
add_compile_options("SHELL:-include ${ROOT}/include/brtcomcent.h")

list(APPEND languages C)

if(HOST STREQUAL windows)
  list(APPEND languages CXX)
endif()

project(${SAVESAVE_NAME} VERSION ${SAVESAVE_VERSION} LANGUAGES ${languages})

include_directories(BEFORE ${ROOT}/include)

configure_file(${ROOT}/savesave.manifest.in
	       ${ROOT}/savesave.manifest)

find_package(zstd CONFIG REQUIRED)
if(BUILD STREQUAL windows)
  get_target_property(LIBZSTD_INCLUDE_DIRS zstd::libzstd
		      INTERFACE_INCLUDE_DIRECTORIES)
  include_directories(${LIBZSTD_INCLUDE_DIRS})
endif()

if(HOST STREQUAL linux)
  file(GLOB platspec_src LIST_DIRECTORIES false uni/*.c)
  set(mainfile ${ROOT}/entry/savesave.c)
else()
  file(GLOB platspec_src LIST_DIRECTORIES false win/*.cpp win/*.c)
  set(mainfile ${ROOT}/entry/savesave.cpp)
endif()
add_library(platspec OBJECT ${platspec_src})

file(GLOB platfree_src LIST_DIRECTORIES false platfree/*.c)
add_library(platfree OBJECT ${platfree_src})

if(HOST STREQUAL linux)
  add_executable(savesave ${mainfile})
else()
  add_executable(savesave WIN32 ${mainfile} ${ROOT}/savesave.rc)
endif()
target_link_libraries(savesave platfree platspec)

if(CONFIG_LINK_LIBZSTD_STATIC)
  target_link_libraries(savesave zstd::libzstd_static)
else()
  target_link_libraries(savesave zstd::libzstd_shared)
endif()

if(HOST STREQUAL windows)
  target_link_libraries(savesave ws2_32 comctl32 shlwapi)
  if (NOT CONFIG_SOURCE_NDEBUG)
    target_link_libraries(savesave dbghelp)
  endif()
endif()

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
