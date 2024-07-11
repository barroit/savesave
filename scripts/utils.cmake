# SPDX-License-Identifier: GPL-3.0-or-later
function (include_dotconfig path)

  if (NOT EXISTS "${path}" OR IS_DIRECTORY $ENV{DOTCONFIG})
    message(FATAL_ERROR "${path} does not exist, run ‘make menuconfig’ first")
  endif()

  file(STRINGS ${path} lines ENCODING UTF-8)

  foreach (line ${lines})
    if (line MATCHES "^#")
      continue()
    endif()

    string(REGEX REPLACE
	   "^(CONFIG_[^=]+)=(\"?)([^\"]*)(\"?)$" "\\1;\\3" res ${line})

    list(GET res 0 key)
    list(GET res 1 val)

    set(${key} ${val} PARENT_SCOPE)
  endforeach()

endfunction()

function (get_cxx_version out)

  get_cmake_property(var VARIABLES)

  foreach(name ${var})
    if (name MATCHES "^CONFIG_CXX_VERSION_(.*)")
      set(${out} ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  endforeach()

endfunction()
