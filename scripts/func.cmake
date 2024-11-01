# SPDX-License-Identifier: GPL-3.0-only

function(test_add_compile_options cond)
  if(${cond})
    list(POP_FRONT ARGV)
    add_compile_options(${ARGV})
  endif()
endfunction()

function(test_add_compile_definitions cond)
  if(${cond})
    list(POP_FRONT ARGV)
    add_compile_definitions(${ARGV})
  endif()
endfunction()

function(test_add_link_options cond)
  if(${cond})
    list(POP_FRONT ARGV)
    add_link_options(${ARGV})
  endif()
endfunction()

function (kselect conf)
  if(NOT DOTCONFIG_VARIABLES)
    message(FATAL_ERROR "BUG!!! DOTCONFIG_VARIABLES not found")
  endif()

  foreach(var ${DOTCONFIG_VARIABLES})
    if (var MATCHES "^${conf}_(.*)")
      set(${conf} ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()
