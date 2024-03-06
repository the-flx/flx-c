# 這些只是為了減少路徑長度... 嗎?
include_directories("${CMAKE_SOURCE_DIR}/include/")  # alway include your own include path

# 這些是一起編譯的檔案
file(GLOB source_files CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/test/*.c"
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(test_${PROJECT_NAME} ${source_files})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
