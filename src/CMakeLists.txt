# 這些只是為了減少路徑長度... 嗎?
include_directories("${CMAKE_SOURCE_DIR}/include/")  # alway include your own include path

add_library(${PROJECT_NAME} SHARED
  "${PROJECT_SOURCE_DIR}/include/stb_ds.h"
  "${PROJECT_SOURCE_DIR}/include/flx.h"
  "${PROJECT_SOURCE_DIR}/src/flx.c")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
