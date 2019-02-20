cmake_minimum_required(VERSION 2.8.11)
project(HeaterMeter)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_BUILD_TYPE Debug)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

set(heatermeter_SRC
  src/main.cpp
  src/heatermeter.cpp
  src/restapi.cpp
  src/lineseries.cpp
  src/tempgraph.cpp
)

# Tell CMake to create the helloworld executable
add_executable(heatermeter ${heatermeter_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(heatermeter Qt5::Widgets)
target_link_libraries(heatermeter Qt5::Network)

# Install the executable
install(TARGETS heatermeter DESTINATION bin)
