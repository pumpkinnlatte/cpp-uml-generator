# core/CMakeLists.txt
#
# Defines the 'core_lib' library, which contains all primary
# application logic: parsing (libclang), the internal data model,
# and diagram exporters (PlantUML).
#
# This library is the "core" of the application and is consumed by
# the 'cli', 'ui', and 'test' targets.
#

# --- Core Library Target ---
#
# This target aggregates all source files for the library.
# Note: Model headers are listed here. If they are truly header-only,
# they are consumed by consumers via target_include_directories.
# If they have matching .cpp files, add those .cpp files here as well.
add_library(core_lib
    # Parser implementation (wraps libclang)
    parser/libclang_parser.cpp
    parser/libclang_parser.h

    # Internal data model
    model/Class.h
    model/Field.h
    model/Method.h
    model/Namespace.h
    model/TranslationUnit.h
    model/TranslationUnit.h

    # Exporter implementation
    exporter/PlantUmlExporter.cpp
    exporter/PlantUmlExporter.h
)

# --- Public Include Interface ---
#
target_include_directories(core_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# --- Include Dependencies ---
#
# These are include paths required *only* to compile 'core_lib.cpp' files.
# They will *not* be propagated to consumers.

target_include_directories(core_lib
    PRIVATE
        ${LLVM_INCLUDE_DIRS} # This variable was set by find_package(LLVM) in the root
)

# --- Link Core LLVM/Clang Libraries ---
#
llvm_map_components_to_libnames(CORE_LLVM_LIBS
    clang-c  # The C API for libclang
    Support  # A core LLVM dependency required by libclang
)

# 'PRIVATE' because consumers of 'core_lib' are fully
# insulated from its internal LLVM dependencies.
target_link_libraries(core_lib
    PRIVATE
        ${CORE_LLVM_LIBS}
)

target_compile_features(core_lib PUBLIC cxx_std_17)