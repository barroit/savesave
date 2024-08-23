# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.21)

set(MAJOR_VERSION  0)
set(PATCH_VERSION  1)
set(TWEAK_VERSION  0)
set(SAVESAVE_VERSION ${MAJOR_VERSION}.${PATCH_VERSION}.${TWEAK_VERSION})

include($ENV{ROOT}/scripts/kconfig.cmake)

# follow the gcc concepts:
#   build is the machine you are building on
#   host  is the machine you are building for
set(BUILD $ENV{BUILD})
if(BUILD STREQUAL linux)
  set(HOST linux)
else()
  set(HOST windows)
endif()

if(BUILD STREQUAL linux)
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  # we do not use cpp on linux
else()
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_CXX_NAME})
endif()

# Let us handle flags
set(CMAKE_BUILD_TYPE "")

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
add_compile_options("SHELL:-include $ENV{AUTOCONF}")
add_compile_options("SHELL:-include $ENV{ROOT}/include/compat.h")

list(APPEND languages C)

if(HOST STREQUAL windows)
  list(APPEND languages CXX)
endif()

project(savesave VERSION ${SAVESAVE_VERSION} LANGUAGES ${languages})

find_package(zstd CONFIG REQUIRED)
if(BUILD STREQUAL windows)
  get_target_property(LIBZSTD_INCLUDE_DIRS zstd::libzstd
		      INTERFACE_INCLUDE_DIRECTORIES)
endif()

include($ENV{ROOT}/scripts/genheader.cmake)

configure_file($ENV{ROOT}/savesave.manifest.in $ENV{ROOT}/savesave.manifest)

include_directories($ENV{ROOT}/include)
if(BUILD STREQUAL windows)
  include_directories(${LIBZSTD_INCLUDE_DIRS})
endif()

if(HOST STREQUAL linux)
  file(GLOB platspec_src LIST_DIRECTORIES false uni/*.c)
  set(mainfile $ENV{ROOT}/entry/savesave.c)
else()
  file(GLOB platspec_src LIST_DIRECTORIES false win/*.cpp)
  set(mainfile $ENV{ROOT}/entry/savesave.cpp)
endif()
add_library(platspec OBJECT ${platspec_src})

file(GLOB platfree_src LIST_DIRECTORIES false platfree/*.c)
add_library(platfree OBJECT ${platfree_src})

if(HOST STREQUAL linux)
  add_executable(savesave ${mainfile})
else()
  add_executable(savesave WIN32 ${mainfile} $ENV{ROOT}/savesave.rc)
endif()
target_link_libraries(savesave platfree platspec)

if(CONFIG_LINK_LIBZSTD_STATIC)
  target_link_libraries(savesave zstd::libzstd_static)
else()
  target_link_libraries(savesave zstd::libzstd_shared)
endif()

if(HOST STREQUAL windows)
  target_link_libraries(savesave ws2_32 comctl32)
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
