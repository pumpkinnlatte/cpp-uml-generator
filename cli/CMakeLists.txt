# cli/CMakeLists.txt

# Crear el ejecutable de la CLI
add_executable(cpp-uml-generator
    main.cpp
)

# Enlazar la CLI con la biblioteca 'core'
target_link_libraries(cpp-uml-generator
    PRIVATE
        core_lib # ¡Aquí es donde consume la biblioteca!
)

# Asegurar el estándar C++
target_compile_features(cpp-uml-generator PRIVATE cxx_std_17)