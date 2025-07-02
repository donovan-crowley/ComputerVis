cmake_minimum_required(VERSION 3.10)
project(ComputerVis)

set(CMAKE_CXX_STANDARD 17)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include headers in src/
include_directories(${OpenCV_INCLUDE_DIRS} src)

# Add the executable and source files
add_executable(ComputerVis
    src/Main.cpp
    src/ObjectTracker.cpp
)

# Link OpenCV libraries
target_link_libraries(ComputerVis ${OpenCV_LIBS})
