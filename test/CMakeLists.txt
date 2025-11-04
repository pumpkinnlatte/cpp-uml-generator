# test/CMakeLists.txt (minimal, no external deps)
enable_testing()

# All test sources under test/ (but we'll remove 'inputs' below)
file(GLOB_RECURSE ALL_TEST_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

# Collect sample/input sources that should NOT be linked into the unit_tests executable.
# These are intended to be parsed by the tests, not compiled into the test runner.
file(GLOB_RECURSE TEST_INPUT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/parser/inputs/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/parser/inputs/*.cc"
)

# Start with the full set minus the input samples
set(TEST_SRCS ${ALL_TEST_SRCS})
if (TEST_INPUT_SRCS)
  list(REMOVE_ITEM TEST_SRCS ${TEST_INPUT_SRCS})
endif()

# Detect sources that contain main() to avoid multiple 'main' definitions.
# We'll remove those from TEST_SRCS and create standalone executables for each.
set(SOURCES_WITH_MAIN)
foreach(src IN LISTS TEST_SRCS)
  # Read file contents (silently ignore read errors)
  file(READ "${src}" _content)
  # Simple heuristic: look for "int main(" (could be false positive in comments, acceptable for tests)
  string(FIND "${_content}" "int main(" _pos)
  if (_pos GREATER -1)
    list(APPEND SOURCES_WITH_MAIN "${src}")
  endif()
endforeach()

# Remove the sources with main from the generic TEST_SRCS list
if (SOURCES_WITH_MAIN)
  list(REMOVE_ITEM TEST_SRCS ${SOURCES_WITH_MAIN})
endif()

# Directory where the test input files live (source tree)
set(TEST_INPUT_DIR "${CMAKE_SOURCE_DIR}/test/parser/inputs")

if(NOT TEST_SRCS AND NOT SOURCES_WITH_MAIN)
  message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
  # Create main test runner executable from sources that do NOT define main
  if (TEST_SRCS)
    add_executable(unit_tests ${TEST_SRCS})

    # Put test executable in a predictable location: <builddir>/test/
    set_target_properties(unit_tests PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    )

    # Link core lib if needed
    target_link_libraries(unit_tests PRIVATE cppuml::core)

    # Register test using the full target-file path so CTest always finds it.
    add_test(NAME unit_tests COMMAND $<TARGET_FILE:unit_tests>)

    # Ensure the test runs with the inputs directory as working directory
    set_tests_properties(unit_tests PROPERTIES WORKING_DIRECTORY "${TEST_INPUT_DIR}")
  endif()

  # For each source that contains main(), create a separate executable and register it as a test.
  foreach(src_main IN LISTS SOURCES_WITH_MAIN)
    # Derive a safe target name from filename, e.g. sample_test_parser
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

    # Run the sample tests with the test inputs directory as working dir as well
    set_tests_properties(${target_name} PROPERTIES WORKING_DIRECTORY "${TEST_INPUT_DIR}")
  endforeach()
endif()