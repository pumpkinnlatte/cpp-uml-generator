# core/CMakeLists.txt - aggregate all .cpp/.cc under core/
file(GLOB_RECURSE CORE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

if(NOT CORE_SRCS)

  message(WARNING "No core sources found in ${CMAKE_CURRENT_SOURCE_DIR}; add source files or create stubs.")

  else()

  add_library(cppuml_core STATIC ${CORE_SRCS})

  target_include_directories(cppuml_core
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include    
      ${CMAKE_CURRENT_SOURCE_DIR}            
  )
  target_compile_features(cppuml_core PUBLIC cxx_std_17)

  add_library(cppuml::core ALIAS cppuml_core)

endif()