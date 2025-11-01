# cli/CMakeLists.txt (minimal)
file(GLOB_RECURSE CLI_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(cppuml_cli ${CLI_SRCS})
target_link_libraries(cppuml_cli PRIVATE cppuml::core)