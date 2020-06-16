#ifndef lineDetector__H__
#define lineDetector__H__

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <vector>
#include <iostream>
//#include <string.h>

class LineDetector {

public:



	// Constructor
	LineDetector();

	~LineDetector();


	struct EdgeConfig {

		// Window names
		const std::string blurThreshWindowName = "1. Blur & Thresholding";
		const std::string edgeWindowName = "1. Canny Edge Detection";
		const std::string morphWindowName = "2. Morphological Operations 1";
		const std::string morphWindowName2 = "3. Morphological Operations 2";
		const std::string houghWindowName = "4. Hough Operations";
		const int screenLength = 1650;
		const int screenWidth = 1080;

		// Edge Detection Operations
		int lowThresh = 20;
		int highThresh = 150;
		int cannyKernel = 0;
		cv::Mat origImg;
		cv::Mat cannyImg;
		cv::Mat blurThreshImg;
		cv::Mat morphImg;
		cv::Mat morphImg2;

		// Blur Parameters
		int gauss_ksize = 0;

		// Binary Thresholding
		int thresBin = 0;

		// Morphological Operations
		int morph_elem_shape = 0;
		int	kernel_morph_size = 0;
		int morphTransformType = 0;
		int morphTransformType_ = 0;
		int	kernel_morph_size_ = 0;

		// Hough transforms

		int houghThreshold = 300;
		//int lowCannyThresh = 20;
		//int highCannyThresh = 150;
		int apertureSize = 1;
		int minVotes = 55;
		int minVotesLim = 200;
		int minLineLengthLimit = 500;
		int minLineLength = 0;
		int maxLineGap = 35;
		int maxLineGapLimit = 500;
		//std::string houghWindowName = "4. Hough transforms";
		int lineThickness = 2;
		std::vector<cv::Vec4i> edgeLines;


	};

	struct HoughConfig {

		// Hough Transformation Parameters
		int houghThreshold = 300;
		int lowCannyThresh = 20;
		int highCannyThresh = 150;
		int apertureSize = 1;
		int minVotes = 55;
		int minVotesLim = 200;
		int minLineLengthLimit = 500;
		int minLineLength = 0;
		int maxLineGap = 35;
		int maxLineGapLimit = 500;
		std::string houghWindowName = "4. Hough transforms";
	};


	struct ConfigParams {
		HoughConfig houghParams;
		EdgeConfig edgeParams;
	};

	// member variables
	ConfigParams configParams;

	/* -------------------- Preprocessing --------------------*/
	
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

	void houghParametersP();
	static void houghPCallback(int, void *userdata);


	//  -- Test functions
	static void displayImg(cv::Mat Img, const std::string title, std::string position, int screenLength, int screenwidth, int numImgs);
	static void PrintFullPath(char * partialPath);
	static void func();
};
#endif 