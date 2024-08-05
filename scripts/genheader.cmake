file(GLOB files $ENV{ROOT}/include/*.in)

foreach(conf ${files})
  cmake_path(GET conf STEM LAST_ONLY name)
  configure_file(${conf} $ENV{ROOT}/include/generated/${name})
endforeach()
