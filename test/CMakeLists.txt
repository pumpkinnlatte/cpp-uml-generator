# Enable CTest for test discovery and running
enable_testing()

# ---- Catch2 Integration ----
include(FetchContent)

# Fetch latest stable Catch2 (adjust tag if needed)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Gather all test source files (excluding parser/inputs/, which are data files for parsing)
file(GLOB_RECURSE ALL_TEST_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

file(GLOB_RECURSE TEST_INPUT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/parser/inputs/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/parser/inputs/*.cc"
)

set(TEST_SRCS ${ALL_TEST_SRCS})
if(TEST_INPUT_SRCS)
  list(REMOVE_ITEM TEST_SRCS ${TEST_INPUT_SRCS})
endif()

# Remove any source files defining main so Catch2's main is used
set(SOURCES_WITH_MAIN)
foreach(src IN LISTS TEST_SRCS)
  file(READ "${src}" _content)
  string(FIND "${_content}" "int main(" _pos)
  if (_pos GREATER -1)
    list(APPEND SOURCES_WITH_MAIN "${src}")
  endif()
endforeach()
if(SOURCES_WITH_MAIN)
  list(REMOVE_ITEM TEST_SRCS ${SOURCES_WITH_MAIN})
endif()

set(TEST_INPUT_DIR "${CMAKE_SOURCE_DIR}/test/parser/inputs")

if(NOT TEST_SRCS)
  message(WARNING "No test sources found for unit_tests in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# ---- Main Catch2 test runner ----
if(TEST_SRCS)
  add_executable(unit_tests ${TEST_SRCS})

  set_target_properties(unit_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
  )

  # Link against Catch2::Catch2WithMain so Catch2 provides the test main()
  target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain cppuml::core)

  # Use Catch2's auto-discovery with ctest
  include(CTest)
  include(${catch2_SOURCE_DIR}/extras/Catch.cmake)
  catch_discover_tests(unit_tests
    WORKING_DIRECTORY "${TEST_INPUT_DIR}"
  )
endif()

# ---- Standalone executables for sources w/ main() ----
foreach(src_main IN LISTS SOURCES_WITH_MAIN)
  get_filename_component(fname_we "${src_main}" NAME_WE)
  string(REGEX REPLACE "[^A-Za-z0-9_]" "_" fname_safe "${fname_we}")
  set(target_name "sample_${fname_safe}")

  add_executable(${target_name} "${src_main}")

  set_target_properties(${target_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
  )

  target_link_libraries(${target_name} PRIVATE cppuml::core)

  add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}>)
  set_tests_properties(${target_name} PROPERTIES WORKING_DIRECTORY "${TEST_INPUT_DIR}")
endforeach()