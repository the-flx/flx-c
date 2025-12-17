# Copyright 2024-2025 the Shne, Jen-Chieh. All rights reserved.
#
# MIT license.

cmake_minimum_required(VERSION 3.16.0)

project("flx" VERSION 0.1.0 LANGUAGES C CXX)

# Make static link library.
add_library(flx STATIC
  include/flx.h
  include/stb_ds.h
  src/flx.c)

# Sub-directories
#add_subdirectory(src)
add_subdirectory(test)

set_property(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  PROPERTY VS_STARTUP_PROJECT test_${PROJECT_NAME})
