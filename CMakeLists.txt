cmake_minimum_required(VERSION 3.0)

PROJECT( AutoDrive )

find_package(OpenCV 4.0 REQUIRED)

include_directories(include/ ${OpenCV_INCLUDE_DIR})

add_library(CalibrationParameters STATIC src/calibParams.cpp include/calibParams.h)
target_link_libraries(CalibrationParameters ${OpenCV_LIBS})

add_library(ObjectDetection STATIC src/objectDetector.cpp include/objectDetector.h)
target_link_libraries(ObjectDetection ${OpenCV_LIBS})

add_library(LineDetection STATIC src/lineDetector.cpp include/lineDetector.h)
target_link_libraries(LineDetection ${OpenCV_LIBS})

add_library(TrafficDetection STATIC src/trafficDetector.cpp include/trafficDetector.h)
target_link_libraries(TrafficDetection ${OpenCV_LIBS})

add_library(ContourDetection STATIC src/contourDetector.cpp include/contourDetector.h)
target_link_libraries(ContourDetection ${OpenCV_LIBS})

add_library(CarTracker STATIC src/carTracker.cpp include/carTracker.h)
target_link_libraries(CarTracker ${OpenCV_LIBS})

add_library(AutoDrive STATIC src/autoDrive.cpp include/autoDrive.h)
target_link_libraries(AutoDrive ${OpenCV_LIBS} CalibrationParameters ObjectDetection LineDetection TrafficDetection ContourDetection CarTracker )

add_executable(AutoDriveTest src/main.cpp src/AutoDriveTest.cpp)
target_link_libraries(AutoDriveTest AutoDrive)





