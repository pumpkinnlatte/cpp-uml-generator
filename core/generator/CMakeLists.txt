# core/generator/CMakeLists.txt
# generator: implementations producing PlantUML/TikZ/LaTeX/Graphviz text

add_library(c2uml_generator STATIC
  # src/PlantUMLGenerator.cpp
  # src/LaTeXGenerator.cpp
)

target_include_directories(c2uml_generator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(c2uml_generator PUBLIC cxx_std_17)

# Depend on model
target_link_libraries(c2uml_generator PUBLIC c2uml::model)

# Expose namespaced alias
add_library(c2uml::generator ALIAS c2uml_generator)