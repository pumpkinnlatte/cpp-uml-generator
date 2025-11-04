cmake_minimum_required(VERSION 3.16)
project(cpp_uml_generator VERSION 0.1.0 LANGUAGES CXX)

# Basic compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # genera compile_commands.json para IDEs

# Options controlling libclang detection
option(LIBCLANG_USE_PKGCONFIG "Use pkg-config to find libclang" ON)
set(LIBCLANG_ROOT "" CACHE PATH "If libclang is installed in a non-standard prefix, set its root here")

# Si el usuario no ha establecido LIBCLANG_ROOT y el path conocido existe, usarlo por defecto
if (NOT DEFINED LIBCLANG_ROOT OR LIBCLANG_ROOT STREQUAL "")
  if (EXISTS "/usr/lib/llvm-18")
    message(STATUS "Auto-detecting LLVM 18 at /usr/lib/llvm-18 and setting LIBCLANG_ROOT")
    set(LIBCLANG_ROOT "/usr/lib/llvm-18" CACHE PATH "Path to LLVM/libclang root" FORCE)
  endif()
endif()

# Intentamos usar pkg-config primero
find_package(PkgConfig QUIET)

if (LIBCLANG_USE_PKGCONFIG AND PkgConfig_FOUND)
    pkg_check_modules(PKG_LIBCLANG QUIET libclang)
endif()

if (PKG_LIBCLANG_FOUND)
    message(STATUS "Found libclang via pkg-config: ${PKG_LIBCLANG_VERSION}")
    set(LIBCLANG_INCLUDE_DIRS ${PKG_LIBCLANG_INCLUDE_DIRS})
    set(LIBCLANG_LIBRARIES ${PKG_LIBCLANG_LIBRARIES})
else()
    # Fallback: buscar encabezado clang-c/Index.h en rutas conocidas o en LIBCLANG_ROOT
    if (LIBCLANG_ROOT)
        find_path(LIBCLANG_INCLUDE_DIR
            NAMES clang-c/Index.h
            PATHS ${LIBCLANG_ROOT}/include ${LIBCLANG_ROOT}/lib/clang/*/include
            NO_DEFAULT_PATH
        )
        find_library(LIBCLANG_LIB
            NAMES clang clang-10 libclang-10 libclang
            PATHS ${LIBCLANG_ROOT}/lib ${LIBCLANG_ROOT}/lib64
            NO_DEFAULT_PATH
        )
    else()
        find_path(LIBCLANG_INCLUDE_DIR NAMES clang-c/Index.h)
        find_library(LIBCLANG_LIB NAMES clang libclang clang-10 libclang-10)
    endif()

    if (LIBCLANG_INCLUDE_DIR AND LIBCLANG_LIB)
        message(STATUS "Found libclang (fallback): include=${LIBCLANG_INCLUDE_DIR} lib=${LIBCLANG_LIB}")
        set(LIBCLANG_INCLUDE_DIRS ${LIBCLANG_INCLUDE_DIR})
        set(LIBCLANG_LIBRARIES ${LIBCLANG_LIB})
    else()
        message(WARNING "libclang not found automatically. Install libclang-dev (or provide LIBCLANG_ROOT), otherwise parsing with libclang will fail.")
    endif()
endif()

# Exponer variables para que subdirs las usen
set(LIBCLANG_INCLUDE_DIRS ${LIBCLANG_INCLUDE_DIRS} CACHE PATH "libclang include dirs")
set(LIBCLANG_LIBRARIES ${LIBCLANG_LIBRARIES} CACHE STRING "libclang libraries")

# Prevent accidental in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not supported. Run: cmake -S . -B build")
endif()

# Options
option(BUILD_UI "Build Qt UI" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Top-level components (keep per-component CMakeLists minimal)
add_subdirectory(core)   # defines library targets for the core
add_subdirectory(cli)    # small executable that uses the core

# UI is optional; the ui/CMakeLists.txt should find/require Qt itself
if(BUILD_UI)
  add_subdirectory(ui)
endif()

# ---- Catch2-powered Test Integration ----
if(BUILD_TESTS)
  # Fetch and enable Catch2 only when building tests (keeps build clean for normal use)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2 # Or latest stable, adjust as needed
  )
  FetchContent_MakeAvailable(Catch2)

  add_subdirectory(test) # Your test/CMakeLists.txt will use Catch2 for runner and test discovery
endif()