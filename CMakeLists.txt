#cmake_minimum_required(VERSION 3.0)

#PROJECT( ShapeDetection )

#include_directories(include)

#add_library(LineDetection STATIC src/lineDetector.cpp include/lineDetector.h)

#target_link_libraries(LineDetection ${OpenCV_LIBS})

#ADD_SUBDIRECTORY( src )


cmake_minimum_required(VERSION 3.0)

project(ShapeDetection)

find_package(OpenCV 4.0 REQUIRED)

include_directories(include/ ${OpenCV_INCLUDE_DIR})

add_library(LineDetection STATIC src/lineDetector.cpp include/lineDetector.h)
target_link_libraries(LineDetection ${OpenCV_LIBS})

add_library(ShapeDetection STATIC src/shapeDetector.cpp include/shapeDetector.h)
target_link_libraries(ShapeDetection ${OpenCV_LIBS} LineDetection)


add_executable(ShapeDetectionTest src/main.cpp src/LineDetectionTest.cpp)
target_link_libraries(ShapeDetectionTest ShapeDetection)





