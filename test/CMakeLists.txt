# test/CMakeLists.txt (minimal, no external deps)
enable_testing()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

if(NOT TEST_SRCS)
  message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
  add_executable(unit_tests ${TEST_SRCS})
  # If your core is necessary, link it (optional)
  target_link_libraries(unit_tests PRIVATE cppuml::core)
  add_test(NAME unit_tests COMMAND unit_tests)
endif()