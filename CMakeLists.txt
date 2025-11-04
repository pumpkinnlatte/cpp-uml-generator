cmake_minimum_required(VERSION 3.18) # Subido a 3.18 por compatibilidad de FetchContent mejorada
project(CppUMLGenerator VERSION 0.1.0 LANGUAGES CXX C)

# --- Configuración Básica del Compilador ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # genera compile_commands.json para IDEs

# --- Opciones del Proyecto ---
# Agrupadas para visibilidad
option(BUILD_UI "Build Qt UI" ON)
option(BUILD_TESTS "Build unit tests" ON)

# --- Prevención de Compilación "In-Source" ---
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not supported. Run: cmake -S . -B build")
endif()

# --- Detección de Dependencias Principales (libclang) ---
#
#
message(STATUS "Looking for LLVM/Clang 18.1.3 or newer...")

find_package(LLVM 18.1.3 # <--- ¡Este es el requisito MÍNIMO!
    REQUIRED
    COMPONENTS
        clang-c # La API C de libclang (Index.h)
        Support # Librería de soporte de LLVM
    CONFIG      # Usar el modo de configuración moderno
)

# Si se encuentra, imprime las rutas (útil para depuración)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} (at ${LLVM_CMAKE_DIR})")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

#
# === NOTA PARA EL USUARIO FINAL ===
# Si CMake no encuentra LLVM, el usuario puede especificar la ruta
# en el comando de cmake, NO editando este archivo.
#
# Ejemplo:
# cmake -S . -B build -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm
#
# Esto es infinitamente más flexible que nuestra lógica anterior.
#

# --- Dependencias de Pruebas (Catch2) ---
if (BUILD_TESTS)
  enable_testing()

  # Si Catch2 no está instalado, usar FetchContent para traerlo automáticamente
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.3  # versión estable
  )
  # No llame a MakeAvailable aquí, hágalo en test/CMakeLists.txt
  # para mantener las variables de Catch2 locales a ese subdirectorio.
  FetchContent_GetProperties(Catch2)
  if(NOT Catch2_POPULATED)
    FetchContent_Populate(Catch2)
  endif()

  set(CATCH2_INCLUDE_DIR ${catch2_SOURCE_DIR}/src)
endif()


# --- Subdirectorios del Proyecto ---
# Estos CMakeLists.txt deben ser ahora más simples

add_subdirectory(core)   # define la biblioteca 'core_lib'
add_subdirectory(cli)    # define el ejecutable 'cli_app'

if(BUILD_UI)
  add_subdirectory(ui)
endif()

if (BUILD_TESTS)
  add_subdirectory(test)
endif()