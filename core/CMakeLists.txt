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

  # If libclang was detected by the top-level CMake, add include dirs and link libraries.
  # The top-level CMake populates LIBCLANG_INCLUDE_DIRS and LIBCLANG_LIBRARIES (CMake cache variables).
  if (LIBCLANG_INCLUDE_DIRS)
    message(STATUS "cppuml_core: adding libclang include dirs: ${LIBCLANG_INCLUDE_DIRS}")
    # LIBCLANG_INCLUDE_DIRS may be a semicolon-separated list
    target_include_directories(cppuml_core PUBLIC ${LIBCLANG_INCLUDE_DIRS})
  endif()

  if (LIBCLANG_LIBRARIES)
    message(STATUS "cppuml_core: linking with libclang: ${LIBCLANG_LIBRARIES}")
    # If pkg-config provided an imported target, prefer it
    if (TARGET PKG_LIBCLANG::libclang)
      target_link_libraries(cppuml_core PUBLIC PKG_LIBCLANG::libclang)
    else()
      target_link_libraries(cppuml_core PUBLIC ${LIBCLANG_LIBRARIES})
    endif()
  endif()

  # Provide a compile-time macro so sources can #ifdef CPPUML_HAS_LIBCLANG
  if (LIBCLANG_INCLUDE_DIRS AND LIBCLANG_LIBRARIES)
    target_compile_definitions(cppuml_core PUBLIC CPPUML_HAS_LIBCLANG=1)
  else()
    target_compile_definitions(cppuml_core PUBLIC CPPUML_HAS_LIBCLANG=0)
  endif()

endif()