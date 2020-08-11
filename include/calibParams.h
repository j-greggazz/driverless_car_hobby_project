#pragma once
#ifndef calibParams__H__
#define calibParams__H__

#include <LineDetector.h>
#include <CarTracker.h>
#include "wtypes.h"

class CalibParams : public LineDetector, CarTracker {

public:

	struct ConfigParams {

		// Window Properties
		const std::string blurThreshWindowName = "1. Blur & Thresholding";
		const std::string roiBoxWindowName = "1. Set Roi-Box";
		const std::string edgeWindowName = "2. Canny Edge Detection";
		const std::string morphWindowName = "3. Morphological Operations 1";
		const std::string morphWindowName2 = "4. Morphological Operations 2";
		const std::string houghWindowName = "5. Hough-Line Operations";
		const std::string houghCWindowName = "6. Hough-Circle Operations";
		const std::string roiBoxWindowName_car = "7. Set Roi-Box";
		int screenHeight;// = 1080; // 768;//1080;
		int screenWidth;// = 1920; //1366;//1920;
		float newCols;
		float newRows;

#if 0:
		// Lane Roi Properties
		int x1_roi = int(400 * 480 / 1080);
		int y1_roi = int(854 * 360 / 1080);
		int recWidth = int(850 * 480 / 1080);
		int recHeight = int(96 * 360 / 1080);
		cv::Rect roi_Bbox = cv::Rect(x1_roi, y1_roi, recWidth, recHeight);
		cv::Mat roiImg;

		// Car Roi Properties
		int x1_roi_car = 0;
		int y1_roi_car = int(629 * 360 / 1080); //845;
		int recWidth_car = int(1489 * 480 / 1080);
		int recHeight_car = int(300 * 360 / 1080); // 141;
		cv::Rect roi_Box_car = cv::Rect(x1_roi_car, y1_roi_car, recWidth_car, recHeight_car);
		cv::Mat roiImg_car;
#endif
		// Lane Roi Properties
		int x1_roi = 400;  //390
		int y1_roi = 854;  //622
		int recWidth = 850;  //1400
		int recHeight = 96;  //405
		cv::Rect roi_Bbox = cv::Rect(x1_roi, y1_roi, recWidth, recHeight);
		cv::Mat roiImg;

		// Car Roi Properties
		int x1_roi_car = 0;  
		int y1_roi_car = 629;   //371
		int recWidth_car = 1489;  //1727
		int recHeight_car = 300;   //439
		cv::Rect roi_Box_car = cv::Rect(x1_roi_car, y1_roi_car, recWidth_car, recHeight_car);
		cv::Mat roiImg_car;

		// Edge Detection Operations
		int lowThresh = 9;
		int highThresh = 23;
		int cannyKernel = 0;
		cv::Mat origImg;
		cv::Mat currImg;
		cv::Mat cannyImg;
		cv::Mat blurThreshImg;

		// Blur Parameters
		int gauss_ksize = 10;

		// Binary Thresholding
		int thresBin = 0;

		// Morphological Operations
		int morph_elem_shape = 0;
		int morphTransformType = 1;
		int	kernel_morph_size = 5;

		int morphTransformType_ = 1;
		int	kernel_morph_size_ = 1;
		cv::Mat morphImg;
		cv::Mat morphImg2;

		// Hough transforms
		cv::Mat houghImg;

		// --Lines
		int houghThreshold = 300;
		int apertureSize = 1;
		int minVotes = 62;
		int minVotesLim = 100;
		int minLineLengthLimit = 100;
		int minLineLength = 56;
		int maxLineGap = 6;
		int maxLineGapLimit = 20;
		int lineThickness = 4;
		std::vector<cv::Vec4i> edgeLines;
		cv::Point line1_pt1, line1_pt2, line2_pt1, line2_pt2;

		// -- Circles
		int centreThresh = 100;
		int circleThickness = 1;
		bool clrChange = true;
		int minRadius = 10;
		int maxRadius = 50;
		int minDistBtwCenters = 10;
		int dp = 2; // Inverse ratio of the accumulator resolution to the image resolution

		// Close-windows 
		bool terminateSetup = false;
	};

	/* -------------------- Preprocessing --------------------*/
	//  -- Setup --
	void getDesktopResolution();
	static void setup(CalibParams& cb,  cv::Mat& img);

	//  -- ROI Setting
	void setLane_ROIBox();
	static void roiLaneCallback(int, void *userdata);
	void setCarDetectionROIBox();
	static void roiCarCallback(int, void *userdata);

	//  -- Edge detection
	void edgeParametersP();
	static void edgeDetectCallback(int, void *userdata);
	void blurThreshParametersP();
	static void blurThreshCallback(int, void *userdata);

	// -- Morphological parameters
	void morphParametersP();
	static void morphCallback(int, void *userdata);
	void morphParametersP2();
	static void morphCallback2(int, void *userdata);

	// -- Hough transforms
	void houghCParameters();
	static void houghC_Callback(int, void *userdata);
	void houghLParametersP();
	static void houghLPCallback(int, void *userdata);

	static void displayImg(cv::Mat Img, const std::string title, int screenWidth, int screenHeight, int imgNum);
	static void drawLines(ConfigParams* configParams, cv::Mat& Img, std::vector<cv::Vec4i> lines, bool laneDetection = false);
	void updateParams();

	preprocessParams getPreprocessParams();
	houghParams getHoughParams();

//private:
	// member variables
	ConfigParams m_configParams;
	preprocessParams m_preprocessVar;
	preprocessImgs m_preprocessImg;
	houghParams m_houghVar;














};
#endif 