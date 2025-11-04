# core/CMakeLists.txt

# Crear la biblioteca 'core'
add_library(core_lib
    # Fuentes del Parser
    parser/libclang_parser.cpp
    
    # Fuentes del Exporter
    exporter/PlantUmlExporter.cpp

    # Fuentes del Modelo (añada aquí los .cpp si los crea)
    # model/Class.cpp
    # ...
)

# Definir la API pública (cabeceras)
target_include_directories(core_lib
    PUBLIC
        # Esto permite que otros objetivos (cli, test) hagan #include "model/Class.h"
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Enlazar el núcleo con libclang usando el objetivo importado
# Esto maneja automáticamente los includes y las bibliotecas.
target_link_libraries(core_lib
    PUBLIC
        # Usamos PUBLIC porque nuestra API (ej. libclang_parser.h)
        # podría exponer tipos de datos de libclang (como CXCursor)
        LLVM::clang-c
        LLVM::Support
)

# Asegurar el estándar C++
target_compile_features(core_lib PRIVATE cxx_std_17)