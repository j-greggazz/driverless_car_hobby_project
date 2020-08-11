set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TST_BINARY ${CMAKE_PROJECT_NAME}_Testing)

set(project_sources parameterised_tests.cpp OpenCV_API_Static_Global_Functions_Tests.cpp fixture_tests.cpp main.cpp)

set(project_headers OpenCV_API_Static_Global_Functions_Tests.h parameterised_tests.h fixture_tests.h) 

add_executable(${TST_BINARY} ${project_sources} ${project_headers})

add_test(NAME ${TST_BINARY} COMMAND ${TST_BINARY})

set( DEPENDENCIES ${OpenCV_LIBS} OPENCV_API_CALLS DashboardTracker CalibrationParameters ObjectDetection LineDetection TrafficDetection ContourDetection CarTracker Qt5::Core Qt5::Gui Qt5::Widgets)


target_link_libraries(${TST_BINARY} PUBLIC ${DEPENDENCIES} gtest ${OpenCV_LIBS}) # target linked library of source_binary

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

