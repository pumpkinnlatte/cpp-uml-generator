# core/CMakeLists.txt (minimal, aggregates sources)
file(GLOB_RECURSE CORE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)
if(NOT CORE_SRCS)
  message(WARNING "No core sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src; add source files or create stubs.")
endif()

add_library(cppuml_core STATIC ${CORE_SRCS})

target_include_directories(cppuml_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(cppuml_core PUBLIC cxx_std_17)

add_library(cppuml::core ALIAS cppuml_core)