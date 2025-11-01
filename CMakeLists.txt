cmake_minimum_required(VERSION 3.16)
project(cpp_uml_generator VERSION 0.1.0 LANGUAGES CXX)

# Basic compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_UI "Build Qt UI" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Prevent accidental in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not supported. Run: cmake -S . -B build")
endif()

# Top-level components (keep per-component CMakeLists minimal)
add_subdirectory(core)   # defines library targets for the core
add_subdirectory(cli)    # small executable that uses the core

# UI is optional; the ui/CMakeLists.txt should find/require Qt itself
if(BUILD_UI)
  add_subdirectory(ui)
endif()

# Tests are optional and must call enable_testing() in this scope
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()