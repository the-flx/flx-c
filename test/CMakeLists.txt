# 這些是一起編譯的檔案

add_executable(test_${PROJECT_NAME}
  "${PROJECT_SOURCE_DIR}/test/main.c"
)

target_link_libraries(test_${PROJECT_NAME}
  PRIVATE flx
)

set_target_properties(test_${PROJECT_NAME}
  PROPERTIES
  CXX_STANDARD 17
)
