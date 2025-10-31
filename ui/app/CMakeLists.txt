# ui/app/CMakeLists.txt
# Minimal Qt-based GUI target. Adjust Qt version to your environment.

cmake_minimum_required(VERSION 3.16)

# Try Qt6 then Qt5
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  set(_QT_LIB Qt6::Widgets)
else()
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
  set(_QT_LIB Qt5::Widgets)
endif()

add_executable(cppuml_gui
  src/main.cpp
  # add your other GUI sources here (mainwindow.cpp, canvas.cpp, etc.)
)

target_include_directories(cppuml_gui PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link to core (make sure core::c2uml alias exists) and Qt
# If your root uses alias c2uml::core, use that. Otherwise link to the core target name.
target_link_libraries(cppuml_gui PRIVATE
  c2uml::core
  ${_QT_LIB}
)

# Enable automatic MOC/UIC/RCC processing
set_target_properties(cppuml_gui PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
  AUTORCC ON
)