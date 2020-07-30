cmake_minimum_required(VERSION 3.0)

PROJECT( DashboardTracker )

# To automatically run MOC when building (Meta Object Compiler)
set(CMAKE_AUTOMOC ON)

# To automatically run UIC when building (User Interface Compiler)
set(CMAKE_AUTOUIC ON)

# To automatically run RCC when building (Resource Compiler)
set(CMAKE_AUTORCC ON)

find_package(OpenCV 4.0 REQUIRED)
#set(OpenCV_FOUND 1)
#set(OpenCV_DIR "C:/Users/gregg/Documents/OpenCV/opencv-4.0.1/opencv-4.0.1/build/install/")
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(include/ ${OpenCV_INCLUDE_DIR})
#set (CMAKE_PREFIX_PATH "C:\\Qt\\5.15.0\\msvc2019_64\\")

#C:\Qt\5.15.0\msvc2019_64
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

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

add_library(DashboardTracker STATIC src/dashboardTracker.cpp include/dashboardTracker.h)
target_link_libraries(DashboardTracker ${OpenCV_LIBS} CalibrationParameters ObjectDetection LineDetection TrafficDetection ContourDetection CarTracker)


set(project_ui include/mainwindow.ui)
qt5_wrap_ui(project_headers_wrapped ${project_ui})
set(project_headers include/mainwindow.h)
qt5_wrap_cpp(project_sources_moc ${project_headers})
set(project_sources src/mainwindow.cpp src/main.cpp src/DashboardTrackerTest.cpp) 
add_executable(DashboardTrackerTest ${project_headers} ${project_sources} ${project_sources_moc} ${project_headers_wrapped})
target_link_libraries(DashboardTrackerTest DashboardTracker Qt5::Core Qt5::Gui Qt5::Widgets)

 
    