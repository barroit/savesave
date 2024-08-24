file(GLOB __files ${ROOT}/include/*.in)

foreach(__conf ${__files})
  cmake_path(GET __conf STEM LAST_ONLY __name)
  configure_file(${__conf} ${GENERATED}/${__name})
endforeach()

unset(__files)
unset(__conf)
unset(__name)
