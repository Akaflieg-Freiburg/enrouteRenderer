# Set up cmake
cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0058 NEW)
cmake_policy(SET CMP0071 NEW)

# Basic information about our project
project(enrouteRenderer VERSION 0.0.1)
add_definitions( -DPROJECT_VERSION="${PROJECT_VERSION}" )

# Include external packages and modules
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include required packages
find_package(Qt6 COMPONENTS Core Positioning Quick Sql REQUIRED)


#
# Set up source files
#

set(SOURCES
  ${PROJECT_NAME}.qrc
  geomaps/Airspace.cpp
  geomaps/Airspace.h
  geomaps/Waypoint.cpp
  geomaps/Waypoint.h
  FlightMap.cpp
  FlightMap.h
  main.cpp
  TileRenderer.cpp
  TileRenderer.h
  units/Distance.cpp
  units/Distance.h
  )

  
#
# Add executable
#

qt_add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Positioning Qt6::Quick Qt6::Sql)
  
qt_add_qml_module(${PROJECT_NAME}
    URI gui
    VERSION 1.0
    QML_FILES
    qml/main.qml
    )
