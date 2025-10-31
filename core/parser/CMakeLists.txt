# core/parser/CMakeLists.txt
# parser: implementation(s) for IParser
# Initially provide a regex prototype parser; later tie in libclang with USE_LIBCLANG option.

# Use a normal target name here (without ::)
add_library(c2uml_parser STATIC
  # add source files when you implement them, e.g.:
  # src/RegexParser.cpp
)

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

# Expose namespaced alias
add_library(c2uml::parser ALIAS c2uml_parser)