# SPDX-License-Identifier: GPL-3.0-only
function (set_choiced_value prefix)
  foreach(var ${DOTCONFIG_VARIABLES})
    if (var MATCHES "^${prefix}_(.*)")
      set(${prefix} ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

execute_process(COMMAND genconfig --header-path ${GENERATED}/autoconf.h
		--config-out ${ROOT}/.config)

file(STRINGS ${ROOT}/.config __lines ENCODING UTF-8)

foreach(__line ${__lines})
  if (__line MATCHES "^#")
    continue()
  endif()

  string(REGEX MATCH "^(CONFIG_[^=]+)=(\"?)([^\"]*)(\"?)$" __unused ${__line})

  set(${CMAKE_MATCH_1} ${CMAKE_MATCH_3})
  list(APPEND DOTCONFIG_VARIABLES "${CMAKE_MATCH_1}")
endforeach()

unset(__lines)
unset(__line)
unset(__unused)
