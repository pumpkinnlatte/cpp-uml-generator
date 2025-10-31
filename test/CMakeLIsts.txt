# core/CMakeLists.txt
# core: small modules (model, parser, generator)
add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(generator)

# Create a normal interface target that aggregates the components
add_library(c2uml_core INTERFACE)

target_link_libraries(c2uml_core INTERFACE
  c2uml::model
  c2uml::parser
  c2uml::generator
)

# Provide a namespaced alias for consumers
add_library(c2uml::core ALIAS c2uml_core)

# API compatibility note: keep core headers Qt-free so core can be used in CLI/tests without Qt.