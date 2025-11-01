# ui/CMakeLists.txt -- support Qt6 with fallback to Qt5; search recursively for sources

# Try Qt6 first (preferred)
find_package(Qt6 COMPONENTS Widgets QUIET)

if(Qt6_FOUND)
  message(STATUS "Found Qt6")
  set(QT_WIDGETS_TARGET Qt6::Widgets)
else()
  find_package(Qt5 COMPONENTS Widgets QUIET)
  if(Qt5_FOUND)
    message(STATUS "Found Qt5")
    set(QT_WIDGETS_TARGET Qt5::Widgets)
  else()
    message(FATAL_ERROR "Qt6 or Qt5 Widgets not found. Install Qt dev packages or set CMAKE_PREFIX_PATH/Qt6_DIR.")
  endif()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# collect UI sources recursively (this will find ui/app/src/main.cpp too)
file(GLOB_RECURSE UI_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)

if(NOT UI_SRCS)
  message(WARNING "No UI sources found in ${CMAKE_CURRENT_SOURCE_DIR}; skipping ui target.")
else()
  add_executable(cppuml_ui ${UI_SRCS})
  target_link_libraries(cppuml_ui
    PRIVATE
      cppuml::core
      ${QT_WIDGETS_TARGET}
  )
endif()