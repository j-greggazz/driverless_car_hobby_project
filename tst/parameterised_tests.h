<<<<<<< HEAD
#pragma once
#ifndef OPENCV_API_PARAMTEST 
#define OPENCV_API_PARAMTEST
#include <OpenCV_API_Static_Global_Functions_Tests.h>
#include "gtest/gtest.h"


class OpenCV_API_ParamTest :public ::testing::TestWithParam<int> {

public:
    int trackerIndex;
    
    
};

#endif //EXAMPLEPROJECT_FORMULA_H


||||||| fa2aa50
=======
#pragma once
#ifndef OPENCV_API_PARAMTEST 
#define OPENCV_API_PARAMTEST
#include <OpenCV_API_Static_Global_Functions_Tests.h>
#include "gtest/gtest.h"


class OpenCV_API_ParamTest :public ::testing::TestWithParam<int> {

public:
    int trackerIndex;
    
    bool tempVar;
};

#endif //EXAMPLEPROJECT_FORMULA_H


>>>>>>> pc_dev
