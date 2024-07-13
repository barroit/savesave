# SPDX-License-Identifier: GPL-3.0
function (set_choiced_value prefix)

  foreach(var ${DOTCONFIG_VARIABLES})
    if (var MATCHES "^${prefix}_(.*)")
      set(${prefix} ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  endforeach()

endfunction()


if (NOT EXISTS "$ENV{DOTCONFIG}" OR IS_DIRECTORY $ENV{DOTCONFIG})
message(FATAL_ERROR "${DOTCONFIG} does not exist, run ‘make menuconfig’ first")
endif()

file(STRINGS $ENV{DOTCONFIG} lines ENCODING UTF-8)

foreach(line ${lines})
  if (line MATCHES "^#")
    continue()
  endif()

  string(REGEX MATCH "^(CONFIG_[^=]+)=(\"?)([^\"]*)(\"?)$" _ ${line})

  set(${CMAKE_MATCH_1} ${CMAKE_MATCH_3})
  list(APPEND DOTCONFIG_VARIABLES "${CMAKE_MATCH_1}")
endforeach()
