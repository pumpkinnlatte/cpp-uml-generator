cmake_minimum_required(VERSION 3.16)
project(cpp-to-uml VERSION 0.1 LANGUAGES CXX)

# Options
option(BUILD_GUI "Build Qt GUI" ON)
option(BUILD_CLI "Build CLI" ON)
option(BUILD_TESTS "Build tests" ON)
option(USE_LIBCLANG "Enable libclang-based parser (optional)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Subprojects
add_subdirectory(core)

if(BUILD_CLI AND EXISTS "${CMAKE_SOURCE_DIR}/cli/CMakeLists.txt")
  add_subdirectory(cli)
endif()

if(BUILD_GUI AND EXISTS "${CMAKE_SOURCE_DIR}/ui/CMakeLists.txt")
  add_subdirectory(ui)
endif()

if(BUILD_TESTS AND EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
  enable_testing()
  add_subdirectory(tests)
endif()