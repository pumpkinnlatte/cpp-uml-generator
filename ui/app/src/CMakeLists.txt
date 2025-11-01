# ui/CMakeLists.txt (minimal Qt Widgets app)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CPPUML_UI_SOURCES
  src/main.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(cppuml_ui ${CPPUML_UI_SOURCES})
target_link_libraries(cppuml_ui PRIVATE cppuml::core Qt5::Widgets)