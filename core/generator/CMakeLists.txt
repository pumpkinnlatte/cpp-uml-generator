# core/generator/CMakeLists.txt
# generator: implementations producing PlantUML/TikZ/LaTeX/Graphviz text

file(GLOB_RECURSE GENERATOR_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
)

if(GENERATOR_SOURCES)
  add_library(c2uml_generator STATIC ${GENERATOR_SOURCES})
  target_include_directories(c2uml_generator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(c2uml_generator PUBLIC cxx_std_17)
  # Depend on model
  target_link_libraries(c2uml_generator PUBLIC c2uml::model)
else()
  add_library(c2uml_generator INTERFACE)
  target_include_directories(c2uml_generator INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_link_libraries(c2uml_generator INTERFACE c2uml::model)
  message(WARNING "No generator implementation sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src. c2uml_generator created as INTERFACE placeholder.")
endif()

# Expose namespaced alias
add_library(c2uml::generator ALIAS c2uml_generator)