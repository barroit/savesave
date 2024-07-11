# SPDX-License-Identifier: GPL-3.0
cmake_minimum_required(VERSION 3.16)

include(scripts/utils.cmake)

include_dotconfig($ENV{DOTCONFIG})

if (CONFIG_CROSS_COMPILING)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 22H2)
endif()

set(CMAKE_CXX_COMPILER ${CONFIG_CXX_NAME})

get_cxx_version(cxx_version)
set(CMAKE_CXX_STANDARD ${cxx_version})
set(CMAKE_CXX_EXTENSIONS true)

if (CONFIG_LINKER_LINK_WMAIN)
list(APPEND CXXFLAGS -municode)
endif()

if (CONFIG_CXX_WARNING_ALL)
list(APPEND CXXFLAGS -Wall)
endif()

message("${CXXFLAGS}")

# message(${CXXFLAGS})

set(MAJOR_VERSION  0)
set(PATCH_VERSION  1)
set(TWEAK_VERSION  0)
set(SAVESAVE_VERSION ${MAJOR_VERSION}.${PATCH_VERSION}.${TWEAK_VERSION})

project(svsave VERSION ${SAVESAVE_VERSION} LANGUAGES CXX)
