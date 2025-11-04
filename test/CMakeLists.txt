# test/CMakeLists.txt

# Hacer que los objetivos de Catch2 estén disponibles (definidos en el raíz)
FetchContent_MakeAvailable(Catch2)

# Crear el ejecutable de prueba
add_executable(run_tests
    # Añada sus archivos de prueba aquí
    parser/test_libclangparser.cpp
    # model/test_model.cpp
    # ...
)

# Enlazar las pruebas con:
# 1. La biblioteca 'core' (para probarla)
# 2. La biblioteca 'Catch2' (para las funciones de prueba)
target_link_libraries(run_tests
    PRIVATE
        core_lib
        Catch2::Catch2WithMain
)

# Añadir la prueba al conjunto de pruebas de CTest (para ejecutar 'ctest')
include(Catch)
catch_discover_tests(run_tests)