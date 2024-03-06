# Copyright 2024 the Shne, Jen-Chieh. All rights reserved.
#
# MIT license.

cmake_minimum_required(VERSION 3.16.0)

# project settings
project("flx" VERSION 0.1.0 LANGUAGES C CXX)

# subdir settings
add_subdirectory(src)
add_subdirectory(test)

set_property(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  PROPERTY VS_STARTUP_PROJECT test_${PROJECT_NAME})
