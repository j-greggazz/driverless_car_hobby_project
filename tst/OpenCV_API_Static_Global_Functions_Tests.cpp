#include "gtest/gtest.h"
#include "opencv_api_calls.h"

using namespace cv;
using namespace std;


TEST(OPENCV_API_FUNCTION_CALLS, checkVideoCaptureWorks) {
    //arrange
    //act
    //assert

    ASSERT_EQ(OPENCV_API_CALLS::check_VideoCaptureWorks(), true) << "Failed to open Video Capture";

}


TEST(OPENCV_API_FUNCTION_CALLS, checkObjectDetectorInitialisation) {
    //arrange
    //act
    //assert
    ASSERT_EQ(OPENCV_API_CALLS::check_ObjectDetectorInitialisation(), true) << "Failed to initialise object detector";

}



