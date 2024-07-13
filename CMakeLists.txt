# SPDX-License-Identifier: GPL-3.0
cmake_minimum_required(VERSION 3.21)

set(MAJOR_VERSION  0)
set(PATCH_VERSION  1)
set(TWEAK_VERSION  0)
set(SAVESAVE_VERSION ${MAJOR_VERSION}.${PATCH_VERSION}.${TWEAK_VERSION})

include(scripts/kconfig.cmake)

if (CONFIG_CROSS_COMPILING OR NOT $ENV{HOST} STREQUAL linux)
set(BUILD windows)
else()
set(BUILD linux)
endif()

if (CONFIG_CROSS_COMPILING)
  set(CMAKE_C_COMPILER ${CONFIG_COMPILER_PREFIX}${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_COMPILER_PREFIX}${CONFIG_CXX_NAME})
elseif(NOT $ENV{HOST} STREQUAL linux) # Windows
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
  set(CMAKE_CXX_COMPILER ${CONFIG_CXX_NAME})
else() # Linux
  set(CMAKE_C_COMPILER ${CONFIG_CC_NAME})
endif()

if (CONFIG_CROSS_COMPILING)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 22H2)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS true)

if (CONFIG_COMPILER_WARNING_ALL)
list(APPEND CMAKE_C_FLAGS -Wall)
list(APPEND CMAKE_CXX_FLAGS -Wall)
endif()

if (CONFIG_COMPILER_WARNING_EXTRA)
list(APPEND CMAKE_C_FLAGS -Wextra)
list(APPEND CMAKE_CXX_FLAGS -Wextra)
endif()

list(APPEND languages C)

if (BUILD STREQUAL windows)
list(APPEND languages CXX)
endif()

project(svsave VERSION ${SAVESAVE_VERSION} LANGUAGES ${languages})

if (BUILD STREQUAL linux)
  file(GLOB platspec_src LIST_DIRECTORIES false src/uni/*.c)
  set(mainfile src/savesave.c)
else()
  file(GLOB platspec_src LIST_DIRECTORIES false src/win/*.cpp)
  set(mainfile src/savesave.cpp)
endif()
add_library(platspec OBJECT ${platspec_src})
target_include_directories(platspec PRIVATE src/)

file(GLOB platfree_src LIST_DIRECTORIES false src/*.c)
list(FILTER platfree_src EXCLUDE REGEX .*savesave.c$)
add_library(platfree OBJECT ${platfree_src})

add_executable(savesave ${mainfile})
target_link_libraries(savesave platfree platspec)

if (CONFIG_LINKER_LINK_WMAIN)
target_link_options(savesave PRIVATE -municode)
endif()

if (CONFIG_USE_UNICODE)
add_compile_definitions(UNICODE)
endif()

if (BUILD STREQUAL windows)
add_compile_definitions(NTDDI_VERSION=NTDDI_WIN10)
endif()
