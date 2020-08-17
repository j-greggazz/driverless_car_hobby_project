#include "gtest/gtest.h"
#include <opencv_api_calls.h>

#define DEBUG_BUILD 1;

int main(int argc, char** argv) {


#if DEBUG_BUILD

    OPENCV_API_CALLS::detectAndDrawLines();

#else
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

#endif


}