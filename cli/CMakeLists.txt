# cli/CMakeLists.txt - find any executable sources under cli/
file(GLOB_RECURSE CLI_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

if(NOT CLI_SRCS)
  message(WARNING "No CLI sources found in ${CMAKE_CURRENT_SOURCE_DIR}; add src/main.cpp or similar.")
else()
  add_executable(cppuml_cli ${CLI_SRCS})
  target_link_libraries(cppuml_cli PRIVATE cppuml::core)
endif()