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

# Gather GUI sources (allow adding sources without changing CMake)
file(GLOB_RECURSE GUI_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
)

if(NOT GUI_SOURCES)
  message(WARNING "No GUI sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src. Creating a generated main so configuration succeeds.")
  # create a simple Qt main in the build dir (won't be committed to repo)
  set(_GENERATED_MAIN ${CMAKE_CURRENT_BINARY_DIR}/generated_main.cpp)
  file(WRITE ${_GENERATED_MAIN}
    "#include <QApplication>\n"
    "#include <QWidget>\n"
    "int main(int argc, char **argv) {\n"
    "  QApplication app(argc, argv);\n"
    "  QWidget w;\n"
    "  w.resize(640,480);\n"
    "  w.setWindowTitle(\"cpp-uml-generator (stub)\");\n"
    "  w.show();\n"
    "  return app.exec();\n"
    "}\n"
  )
  list(APPEND GUI_SOURCES ${_GENERATED_MAIN})
endif()

add_executable(cppuml_gui
  ${GUI_SOURCES}
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