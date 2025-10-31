# core/parser/CMakeLists.txt
# parser: implementation(s) for IParser
# Initially provide a regex prototype parser; later tie in libclang with USE_LIBCLANG option.

# Collect sources if present
file(GLOB_RECURSE PARSER_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
)

if(PARSER_SOURCES)
  add_library(c2uml_parser STATIC ${PARSER_SOURCES})
  target_include_directories(c2uml_parser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(c2uml_parser PUBLIC cxx_std_17)

  # link libclang optionally (user can enable with -DUSE_LIBCLANG=ON)
  if(USE_LIBCLANG)
    find_package(Clang REQUIRED CONFIG) # may require adjusting for your platform
    target_compile_definitions(c2uml_parser PRIVATE USE_LIBCLANG)
    target_link_libraries(c2uml_parser PRIVATE clang::clang)
  endif()

  # Make parser depend on the model interface headers
  target_link_libraries(c2uml_parser PUBLIC c2uml::model)
else()
  # No implementation sources yet: create an INTERFACE placeholder so consumers can still configure
  add_library(c2uml_parser INTERFACE)
  target_include_directories(c2uml_parser INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_link_libraries(c2uml_parser INTERFACE c2uml::model)
  message(WARNING "No parser implementation sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src. c2uml_parser created as INTERFACE placeholder.")
endif()

# Expose namespaced alias
add_library(c2uml::parser ALIAS c2uml_parser)