# core/model/CMakeLists.txt
# model: value-type headers only (header-only library)
add_library(c2uml_model INTERFACE)

target_include_directories(c2uml_model INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Expose a namespaced alias for consumers
add_library(c2uml::model ALIAS c2uml_model)