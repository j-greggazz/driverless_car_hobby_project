#include <lineDetector.h>
using namespace cv;
using namespace std;


LineDetector::LineDetector() {
	struct ConfigParams configParams;
}

LineDetector:: ~LineDetector() {}; 

void LineDetector::houghCParameters() {

	namedWindow(this->configParams.edgeParams.houghCWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.edgeParams.houghCWindowName, int(2 * this->configParams.edgeParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.edgeParams.houghCWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("minRadius", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.minRadius, int(this->configParams.edgeParams.minRadius * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("maxRadius", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.maxRadius, int(this->configParams.edgeParams.maxRadius * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("minDistBtw", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.minDistBtwCenters, int(this->configParams.edgeParams.minDistBtwCenters * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("centreThresh", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.centreThresh, int(this->configParams.edgeParams.centreThresh * 2.5), houghC_Callback, &this->configParams.edgeParams);

	waitKey();
	destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.houghCWindowName);
};


void LineDetector::houghC_Callback(int, void *userdata) {

	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	Mat houghImg = edgeConfig->morphImg2.clone();
	/*
	// 1. Determine hough lines:
	vector<Vec4i> lines;
	HoughLinesP(houghImg, lines, 1, CV_PI / 180, edgeConfig->minVotes, edgeConfig->minLineLength, edgeConfig->maxLineGap);
	edgeConfig->edgeLines = lines;
	Mat houghTransform = edgeConfig->origImg.clone();
	*/
	// 2. Determine elliptical shapes:
	vector<Vec3f> circles;
	Mat houghTransform = edgeConfig->origImg.clone();
	// int minRadiusOffset = edgeConfig->minRadius + 
	cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, edgeConfig->dp, edgeConfig->minDistBtwCenters, edgeConfig->highThresh, edgeConfig->centreThresh, edgeConfig->minRadius, edgeConfig->maxRadius);

	int img_num = 4;
	//drawLines(edgeConfig, houghTransform, lines);
	drawCircles(edgeConfig, houghTransform, circles);
	displayImg(houghTransform, edgeConfig->houghCWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
}


void LineDetector::houghLParametersP() {

	namedWindow(this->configParams.edgeParams.houghWindowName, WINDOW_AUTOSIZE);	
	moveWindow(this->configParams.edgeParams.houghWindowName, int(this->configParams.edgeParams.screenWidth / 3), int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.houghWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("minVotes", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minVotes, this->configParams.edgeParams.minVotesLim, houghLPCallback, &this->configParams.edgeParams);
	createTrackbar("minLength", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minLineLength, this->configParams.edgeParams.minLineLengthLimit, houghLPCallback, &this->configParams.edgeParams);
	createTrackbar("maxGap", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.maxLineGap, this->configParams.edgeParams.maxLineGapLimit, houghLPCallback, &this->configParams.edgeParams);
	
	houghCParameters();
	waitKey();
	destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.houghWindowName);
};


void LineDetector::houghLPCallback(int, void *userdata) {
	
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	Mat houghImg = edgeConfig->morphImg2.clone();
	
	// 1. Determine hough lines:
	vector<Vec4i> lines;
	HoughLinesP(houghImg, lines, 1, CV_PI / 180, edgeConfig->minVotes, edgeConfig->minLineLength, edgeConfig->maxLineGap);
	edgeConfig -> edgeLines = lines;
	Mat houghTransform = edgeConfig->origImg.clone();
	/*
	// 2. Determine elliptical shapes:
	vector<Vec3f> circles;
	cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, edgeConfig->dp, edgeConfig->minDistBtwCenters, edgeConfig->highThresh, edgeConfig->centreThresh, edgeConfig->minRadius, edgeConfig->maxRadius);
	*/
	int img_num = 3;
	drawLines(edgeConfig, houghTransform, lines);
	//drawCircles(edgeConfig, houghTransform, circles);
	displayImg(houghTransform, edgeConfig->houghWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	houghC_Callback(0, edgeConfig);
}

void LineDetector::drawLines(EdgeConfig* edgeParams, cv::Mat& img, std::vector<cv::Vec4i> lines){
	for (auto it = lines.begin(); it != lines.end(); ++it) {
		Point a, b;
		auto line = *it;
		int x = line[0] + line[1] + line[2] + line[3];

		if (x != 0) {
			a.x = line[0];
			a.y = line[1];
			b.x = line[2];
			b.y = line[3];
			cv::line(img, a, b, Scalar(0, 0, 255), edgeParams->lineThickness, LINE_AA);
		}
	}
}

void LineDetector::drawCircles(EdgeConfig* edgeParams, cv::Mat& img, const std::vector<cv::Vec3f>& circles) {

	for (size_t i = 0; i < circles.size(); i++)
	{
		int radius = cvRound(circles[i][2]);
		Point centre(cvRound(circles[i][0]), cvRound(circles[i][1]));

		if (edgeParams->clrChange) {
			if (i % 2 == 0) {
				circle(img, centre, 3, Scalar(0, 255, 0), edgeParams->circleThickness, 8, 0);
				circle(img, centre, radius, Scalar(0, 255, 0), edgeParams->circleThickness, 8, 0);
			}
			else {
				circle(img, centre, 3, Scalar(0, 0, 255), edgeParams->circleThickness, 8, 0);
				circle(img, centre, radius, Scalar(0, 0, 255), edgeParams->circleThickness, 8, 0);
			}
		}
		else {
			circle(img, centre, 3, Scalar(255, 0, 0), edgeParams->circleThickness, 8, 0);
			circle(img, centre, radius, Scalar(255, 0, 0), edgeParams->circleThickness, 8, 0);
		}

	}
}

void LineDetector::morphParametersP2() {

	namedWindow(this->configParams.edgeParams.morphWindowName2, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->configParams.edgeParams.morphWindowName2, int(this->configParams.edgeParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.edgeParams.morphWindowName2, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.morphTransformType_, 8, morphCallback2, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.kernel_morph_size_, 5, morphCallback2, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);
	
	houghLParametersP();
	waitKey();
	destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.morphWindowName2);
}

void LineDetector::morphCallback2(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType_ + 1;

	Mat morphImg2 = edgeConfig->morphImg.clone();
	
	// Morphological Operations:
	if (operation > 3) {
		Mat str_element = getStructuringElement(edgeConfig->morph_elem_shape, Size(2 * edgeConfig->kernel_morph_size_ + 1, 2 * edgeConfig->kernel_morph_size_ + 1), Point(edgeConfig->kernel_morph_size_, edgeConfig->kernel_morph_size_));
		operation -= 2;
		morphologyEx(morphImg2, morphImg2, operation, str_element);
	}
	else if (operation == 1) {
		//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
		//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
		//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		Mat element = getStructuringElement(MORPH_CROSS, Size(edgeConfig->kernel_morph_size_ + 1, edgeConfig->kernel_morph_size_ + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(morphImg2, morphImg2, element, Point(-1, -1), 2, 1, 1);
	}

	else if (operation == 2) {
		//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
		//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
		//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		Mat element = getStructuringElement(MORPH_RECT, Size(edgeConfig->kernel_morph_size_ + 1, edgeConfig->kernel_morph_size_ + 1), Point(edgeConfig->kernel_morph_size_, edgeConfig->kernel_morph_size_));
		dilate(morphImg2, morphImg2, element);
	}

	if (morphImg2.empty()) {
		morphImg2 = edgeConfig->origImg;
	}
	edgeConfig->morphImg2 = morphImg2;

	int img_num = 2;
	displayImg(morphImg2, edgeConfig->morphWindowName2, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	houghLPCallback(0, edgeConfig);
}


void LineDetector::morphParametersP() {  
	
	namedWindow(this->configParams.edgeParams.morphWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.edgeParams.morphWindowName, 0, int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.morphWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morphTransformType, 8, morphCallback, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.kernel_morph_size, 5, morphCallback, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);
	morphParametersP2();
	waitKey();
	destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.morphWindowName);
}

void LineDetector::morphCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType + 1;

	Mat morphImg = edgeConfig->cannyImg.clone();
	
	// Morphological Operations:
	if (operation > 3) {
		Mat str_element = getStructuringElement(edgeConfig->morph_elem_shape, Size(2 * edgeConfig->kernel_morph_size + 1, 2 * edgeConfig->kernel_morph_size + 1), Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		operation -= 2;
		morphologyEx(morphImg, morphImg, operation, str_element);
	}
	else if (operation == 1) {
		//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
		//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
		//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		Mat element = getStructuringElement(MORPH_CROSS, Size(edgeConfig->kernel_morph_size + 1, edgeConfig->kernel_morph_size + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
	}

	else if (operation == 2) {
		//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
		//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
		//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		Mat element = getStructuringElement(MORPH_RECT, Size(edgeConfig->kernel_morph_size+1, edgeConfig->kernel_morph_size+1), Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		dilate(morphImg, morphImg, element);
	}

	if (morphImg.empty()) {
		morphImg = edgeConfig->origImg;
	}
	edgeConfig->morphImg = morphImg;

	int img_num = 1;
	displayImg(morphImg, edgeConfig->morphWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	morphCallback2(0, edgeConfig);
}


void LineDetector::edgeParametersP() {

	namedWindow(this->configParams.edgeParams.edgeWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeParams.edgeWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));
	moveWindow(this->configParams.edgeParams.edgeWindowName, 0, 0);
	
	createTrackbar("BlurLevel", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.gauss_ksize, 15, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyLow", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.lowThresh, 100, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyHigh", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.highThresh, 150, edgeDetectCallback, &this->configParams.edgeParams);

	morphParametersP();
	waitKey();
	destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.edgeWindowName);
};

void LineDetector::edgeDetectCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType + 1;
	int cannyKernelSize = edgeConfig->cannyKernel + 3;
	cv::Mat cannyImg;
	Mat img_ = edgeConfig->origImg.clone();

	// Blur:
	if (edgeConfig->gauss_ksize > 0) {
		cvtColor(img_, cannyImg, COLOR_BGR2GRAY);
		blur(cannyImg, cannyImg, Size(edgeConfig->gauss_ksize, edgeConfig->gauss_ksize), Point(-1, -1));
	}
	// Edge Detection:
	if (!cannyImg.empty()) {
		Canny(cannyImg, cannyImg, edgeConfig->lowThresh, edgeConfig->highThresh, cannyKernelSize);
		edgeConfig->cannyImg = cannyImg.clone();
		int img_num = 0;
		displayImg(cannyImg, edgeConfig->edgeWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
		morphCallback(0, edgeConfig);
	}

	if (0) {
		if (img_.empty()) {
			img_ = edgeConfig->origImg;
			cannyImg = img_.clone();
		}
		else {

			// Edge Detection:
			Canny(img_, cannyImg, edgeConfig->lowThresh, edgeConfig->highThresh, cannyKernelSize);
			edgeConfig->cannyImg = cannyImg.clone();

		}
	}

}


void LineDetector::blurThreshParametersP() {

	namedWindow(this->configParams.edgeParams.blurThreshWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeParams.blurThreshWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));
	moveWindow(this->configParams.edgeParams.blurThreshWindowName, 0, 0);

	createTrackbar("BlurLevel", this->configParams.edgeParams.blurThreshWindowName, &this->configParams.edgeParams.gauss_ksize, 10, blurThreshCallback, &this->configParams.edgeParams);
	//createTrackbar("TheshLevel", this->configParams.edgeParams.BlurThreshWindowName, &this->configParams.edgeParams.thresBin, 255, blurThreshCallback, &this->configParams.edgeParams);

	edgeParametersP();
	waitKey();
	//destroyAllWindows();
	destroyWindow(this->configParams.edgeParams.blurThreshWindowName);
}

void LineDetector::blurThreshCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType + 1;
	int cannyKernelSize = edgeConfig->cannyKernel + 3;
	cv::Mat blurThreshImg = edgeConfig->origImg.clone();
	Mat img_ = edgeConfig->origImg.clone();  

	cvtColor(img_, blurThreshImg, COLOR_BGR2GRAY);

	// Blur:
	if (edgeConfig->gauss_ksize > 0) {
		blur(blurThreshImg, blurThreshImg, Size(edgeConfig->gauss_ksize, edgeConfig->gauss_ksize), Point(-1, -1));
	}
	// Thresholding

	//threshold(blurThreshImg, blurThreshImg, edgeConfig->thresBin, 255, 0);
	//cvtColor(blurThreshImg, blurThreshImg, COLOR_GRAY2BGR);
	
	edgeConfig->blurThreshImg = blurThreshImg.clone();
	
	int img_num = 0;

	displayImg(blurThreshImg, edgeConfig->blurThreshWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	edgeDetectCallback(0, edgeConfig);
}


void LineDetector::displayImg(Mat img, const std::string title, int screenWidth, int screenHeight, int img_num) {

	//double img_width = img.cols;  //obtain size of image and halve it in order to fit 
	//double img_height = img.rows;
	int maxHeight = int(screenHeight / 2);
	int maxWidth = int(screenWidth / 3);


	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;		
	}

	else {
		new_rows = maxHeight;
	}
	
	
	/*
	int newCols;
	int newRows; 
	int diff_height = maxHeight - img_height;
	int diff_width = maxWidth - img_width;

	if (diff_height > diff_width) {
		newCols = maxWidth;
		newRows = img.rows * newCols / img.cols;
	}
	else {
		newRows = maxHeight;
		newCols = img.cols * newRows / img.rows;
	}
	*/

	if (img_num != -1) {
		
		Mat img_;
		//cout << float(newRows) / float(img.rows) << " " << float(newCols) / float(img.cols);
		resize(img, img_, cv::Size(), float(new_rows) / float(img.rows), float(new_cols) / float(img.cols));

		if (img_num == 0) {
			moveWindow(title, 0, 0);
		}

		else if (img_num == 1) {
			moveWindow(title, 0, 1.1 * maxHeight);
		}

		else if (img_num == 2) {
			moveWindow(title, new_cols, 0);
		}

		else if (img_num == 3) {
			moveWindow(title, new_cols, 1.1 * maxHeight);
		}

		else if (img_num == 4) {
			moveWindow(title, 2 * new_cols, 0);
		}
	

		resizeWindow(title, img_.cols, img_.rows);
		imshow(title, img_);
	}

	else {
		imshow(title, img);
	}
}


















void LineDetector::PrintFullPath(char * partialPath)
{
	char full[_MAX_PATH];
	if (_fullpath(full, partialPath, _MAX_PATH) != NULL)
		printf("Full path is: %s\n", full);
	else
		printf("Invalid path\n");
}


void LineDetector::func() {

	//PrintFullPath(".\\");
	std::cout << "dog" << std::endl;

	cv::Mat image;
	image = cv::imread("../data/sample_car.jpg");   // Read the file

	if (!image.data)                              // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		//return -1;
	}

	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);// Create a window for display.
	cv::imshow("Display window", image);                   // Show our image inside it.

	cv::waitKey();                                       // Wait for a keystroke in the window
	//system("pause");
}

