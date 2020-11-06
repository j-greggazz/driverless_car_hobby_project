
### ~~~~~~~~~~~~ General  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_minimum_required(VERSION 3.10)

project( DashboardTracker )

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # "https://cmake.org/cmake/help/v3.0/variable/CMAKE_INCLUDE_CURRENT_DIR.html" ( QT RUNS PROPERLY WHEN THIS IS SET? )

set (Boost_NO_BOOST_CMAKE ON)

### ~~~~~~~~~~~~ Find Packages ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCV 4.0 REQUIRED)
find_package(Boost 1.74 COMPONENTS filesystem REQUIRED)

if(Boost_FOUND)
	message("Boost is found despite above errors")
endif()

### ~~~~~~~~~~~~ General Include/Add ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# IMPORTANT - "include_directories" must come before "add_subdirectory" - otherwise include files not found!
include_directories(include ${OpenCV_INCLUDE_DIR} ${Boost_INCLUDE_DIR})


add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(clientApp)
add_subdirectory(lib/googletest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
