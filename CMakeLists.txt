# [project root]/CMakeLists.txt

#
# CppUMLGenerator Root CMake Configuration
#
# This file manages project-wide settings, locates top-level dependencies,
# and orchestrates the build by including the subdirectories for the
# core library, CLI, UI, and tests.
#

cmake_minimum_required(VERSION 3.18) # Raised to 3.18 for improved FetchContent compatibility
project(CppUMLGenerator VERSION 0.1.0 LANGUAGES CXX C)

# --- Compiler and Build Standards ---
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)         # Disables GNU extensions (e.g., -std=gnu++17) for better portability
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Required for shared libraries, good practice for static ones
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json for IDEs and tools (clangd, etc.)

# --- Project Options ---
#
option(BUILD_UI "Build Qt UI" ON)
option(BUILD_TESTS "Build unit tests" ON)

# --- Build Configuration ---
#
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not supported. Run: cmake -S . -B build")
endif()

# --- Core Dependency: LLVM/Clang ---
#
# Locate the LLVM/Clang 18.1.3+ libraries.
#
message(STATUS "Looking for LLVM/Clang 18.1.3 or newer...")

find_package(LLVM 18.1.3
    REQUIRED  # Fail configuration if LLVM is not found
    CONFIG    # Use LLVMConfig.cmake (modern)
    # NOTE: Components are *not* requested here.
    # Sub-projects (i.e., core_lib) are responsible for mapping the
    # specific components they need (e.g., 'clang-c', 'Support')
    # using llvm_map_components_to_libnames(). This is more modular.
)

# User Feedback on the located LLVM installation.
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} (at ${LLVM_CMAKE_DIR})")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

#
# === User Note ===
#
# If CMake doesnt finds LLVM, the route can be specified on the cmake command.
# DO NOT EDIT THE FILE
#
# Example:
# cmake -S . -B build -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm
#

# --- Test Dependencies ---
#
# Configure test dependencies only if tests are enabled.
# We use the modern FetchContent module to acquire Catch2.
#
if (BUILD_TESTS)
  enable_testing() # Enables CTest

  # Ensure the FetchContent module is available
  include(FetchContent)

  # DECLARE the dependency. This does *not* download it.
  # The 'test/CMakeLists.txt' file will call FetchContent_MakeAvailable(Catch2),
  # which will trigger the download only if BUILD_TESTS is ON.
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.3  # Pin to a specific, stable version
  )
endif()

# --- Project Structure ---
#
# Add the primary components of the application.
# The 'core' and 'cli' targets are mandatory.
#
add_subdirectory(core)   # Defines the 'core_lib' library
add_subdirectory(cli)    # Defines the 'cli_app' executable

# Conditionally add optional components based on the build options.
if(BUILD_UI)
  add_subdirectory(ui)
endif()

if (BUILD_TESTS)
  add_subdirectory(test)
endif()