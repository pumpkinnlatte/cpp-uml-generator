# test/CMakeLists.txt (minimal GoogleTest via FetchContent)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

add_executable(unit_tests ${TEST_SRCS})
target_link_libraries(unit_tests PRIVATE cppuml::core GTest::gtest_main)

add_test(NAME unit_tests COMMAND unit_tests)
