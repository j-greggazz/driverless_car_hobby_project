

### ~~~~~~~~~~~~ BUILD RELEVANT LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

add_library(CalibrationParameters STATIC calibParams.cpp ../include/calibParams.h)
target_link_libraries(CalibrationParameters ${OpenCV_LIBS})

add_library(ObjectDetection STATIC objectDetector.cpp ../include/objectDetector.h)
target_link_libraries(ObjectDetection ${OpenCV_LIBS})

add_library(LineDetection STATIC lineDetector.cpp ../include/lineDetector.h)
target_link_libraries(LineDetection ${OpenCV_LIBS})

add_library(TrafficDetection STATIC trafficDetector.cpp ../include/trafficDetector.h)
target_link_libraries(TrafficDetection ${OpenCV_LIBS})

add_library(ContourDetection STATIC contourDetector.cpp ../include/contourDetector.h)
target_link_libraries(ContourDetection ${OpenCV_LIBS})

add_library(CarTracker STATIC carTracker.cpp ../include/carTracker.h)
target_link_libraries(CarTracker ${OpenCV_LIBS})

add_library(OPENCV_API_CALLS STATIC opencv_api_calls.cpp ../include/opencv_api_calls.h)
target_link_libraries(OPENCV_API_CALLS ${OpenCV_LIBS})

add_library(DashboardTracker STATIC dashboardTracker.cpp ../include/dashboardTracker.h)
target_link_libraries(DashboardTracker ${OpenCV_LIBS} CalibrationParameters ObjectDetection LineDetection TrafficDetection ContourDetection CarTracker)


### ~~~~~~~~~~~~ QT SETTINGS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# To automatically run MOC when building (Meta Object Compiler)
set(CMAKE_AUTOMOC ON)

# To automatically run UIC when building (User Interface Compiler)
set(CMAKE_AUTOUIC ON)

# To automatically run RCC when building (Resource Compiler)
set(CMAKE_AUTORCC ON)

set(project_ui ../include/mainwindow.ui)
qt5_wrap_ui(project_headers_wrapped ${project_ui})
set(project_headers ../include/mainwindow.h)
qt5_wrap_cpp(project_sources_moc ${project_headers})


### ~~~~~~~~~~~~ CREATE EXECUTABLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(project_sources mainwindow.cpp main.cpp DashboardTrackerRun.cpp)
add_executable(DashboardTrackerRun ${project_headers} ${project_sources} ${project_sources_moc} ${project_headers_wrapped})
target_link_libraries(DashboardTrackerRun DashboardTracker Qt5::Core Qt5::Gui Qt5::Widgets)