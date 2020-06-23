#include <lineDetector.h>

using namespace cv;
using namespace std;


LineDetector::LineDetector() {
	struct ConfigParams configParams;
}
LineDetector:: ~LineDetector() {};

/* -------------------- Preprocessing --------------------*/

void LineDetector::houghCParameters() {

	namedWindow(this->configParams.edgeParams.houghCWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.edgeParams.houghCWindowName, int(2 * this->configParams.edgeParams.screenWidth / 3), int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.houghCWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("minRadius", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.minRadius, int(this->configParams.edgeParams.minRadius * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("maxRadius", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.maxRadius, int(this->configParams.edgeParams.maxRadius * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("minDistBtw", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.minDistBtwCenters, int(this->configParams.edgeParams.minDistBtwCenters * 2.5), houghC_Callback, &this->configParams.edgeParams);
	createTrackbar("cntrThresh", this->configParams.edgeParams.houghCWindowName, &this->configParams.edgeParams.centreThresh, int(this->configParams.edgeParams.centreThresh * 2.5), houghC_Callback, &this->configParams.edgeParams);

	waitKey();
	destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.houghCWindowName);
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

	int img_num = 5;
	//drawLines(edgeConfig, houghTransform, lines);
	drawCircles(edgeConfig, houghTransform, circles);
	displayImg(houghTransform, edgeConfig->houghCWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
}

void LineDetector::houghLParametersP() {

	namedWindow(this->configParams.edgeParams.houghWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.edgeParams.houghWindowName, int(2 * this->configParams.edgeParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.edgeParams.houghWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("minVotes", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minVotes, this->configParams.edgeParams.minVotesLim, houghLPCallback, &this->configParams.edgeParams);
	createTrackbar("minLength", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minLineLength, this->configParams.edgeParams.minLineLengthLimit, houghLPCallback, &this->configParams.edgeParams);
	createTrackbar("maxGap", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.maxLineGap, this->configParams.edgeParams.maxLineGapLimit, houghLPCallback, &this->configParams.edgeParams);

	houghCParameters();
	waitKey();
	destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.houghWindowName);
};
void LineDetector::houghLPCallback(int, void *userdata) {

	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	Mat houghImg = edgeConfig->morphImg2.clone();

	// 1. Determine hough lines:
	vector<Vec4i> lines;
	HoughLinesP(houghImg, lines, 1, CV_PI / 180, edgeConfig->minVotes, edgeConfig->minLineLength, edgeConfig->maxLineGap);
	edgeConfig->edgeLines = lines;
	Mat houghTransform = edgeConfig->origImg.clone();
	/*
	// 2. Determine elliptical shapes:
	vector<Vec3f> circles;
	cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, edgeConfig->dp, edgeConfig->minDistBtwCenters, edgeConfig->highThresh, edgeConfig->centreThresh, edgeConfig->minRadius, edgeConfig->maxRadius);
	*/
	int img_num = 4;
	drawLines(edgeConfig, houghTransform, lines);
	//drawCircles(edgeConfig, houghTransform, circles);
	displayImg(houghTransform, edgeConfig->houghWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	houghC_Callback(0, edgeConfig);
}

void LineDetector::morphParametersP2() {

	namedWindow(this->configParams.edgeParams.morphWindowName2, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->configParams.edgeParams.morphWindowName2, int(this->configParams.edgeParams.screenWidth / 3), int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.morphWindowName2, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.morphTransformType_, 8, morphCallback2, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.kernel_morph_size_, 5, morphCallback2, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);

	houghLParametersP();
	waitKey();
	destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.morphWindowName2);
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

	int img_num = 3;
	displayImg(morphImg2, edgeConfig->morphWindowName2, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	houghLPCallback(0, edgeConfig);
}

void LineDetector::morphParametersP() {

	namedWindow(this->configParams.edgeParams.morphWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.edgeParams.morphWindowName, int(this->configParams.edgeParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.edgeParams.morphWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morphTransformType, 8, morphCallback, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.kernel_morph_size, 5, morphCallback, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);
	morphParametersP2();
	waitKey();
	destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.morphWindowName);
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

		Mat element = getStructuringElement(MORPH_RECT, Size(edgeConfig->kernel_morph_size + 1, edgeConfig->kernel_morph_size + 1), Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		dilate(morphImg, morphImg, element);
	}

	if (morphImg.empty()) {
		morphImg = edgeConfig->origImg;
	}
	edgeConfig->morphImg = morphImg;

	int img_num = 2;
	displayImg(morphImg, edgeConfig->morphWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	morphCallback2(0, edgeConfig);
}

void LineDetector::edgeParametersP() {

	namedWindow(this->configParams.edgeParams.edgeWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeParams.edgeWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));
	moveWindow(this->configParams.edgeParams.edgeWindowName, 0, int(this->configParams.edgeParams.screenHeight / 2));

	createTrackbar("BlurLevel", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.gauss_ksize, 15, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyLow", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.lowThresh, 100, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyHigh", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.highThresh, 150, edgeDetectCallback, &this->configParams.edgeParams);

	morphParametersP();
	waitKey();
	destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.edgeWindowName);
};
void LineDetector::edgeDetectCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType + 1;
	int cannyKernelSize = edgeConfig->cannyKernel + 3;
	cv::Mat cannyImg;
	Mat img_ = edgeConfig->roiImg.clone();

	// Blur:
	if (edgeConfig->gauss_ksize > 0) {
		cvtColor(img_, cannyImg, COLOR_BGR2GRAY);
		blur(cannyImg, cannyImg, Size(edgeConfig->gauss_ksize, edgeConfig->gauss_ksize), Point(-1, -1));
	}
	// Edge Detection:
	if (!cannyImg.empty()) {
		Canny(cannyImg, cannyImg, edgeConfig->lowThresh, edgeConfig->highThresh, cannyKernelSize);
		edgeConfig->cannyImg = cannyImg.clone();
		int img_num = 1;
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

void LineDetector::setLane_ROIBox() {
	namedWindow(this->configParams.edgeParams.roiBoxWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeParams.roiBoxWindowName, int(configParams.edgeParams.newCols), int(configParams.edgeParams.newRows));
	moveWindow(this->configParams.edgeParams.roiBoxWindowName, 0, 0);
	size_t max_val_x = this->configParams.edgeParams.origImg.cols;
	size_t max_val_y = this->configParams.edgeParams.origImg.rows;

	createTrackbar("X1-Pos", this->configParams.edgeParams.roiBoxWindowName, &this->configParams.edgeParams.x1_roi, max_val_x, roiLaneCallback, &this->configParams.edgeParams);
	createTrackbar("RecWidth", this->configParams.edgeParams.roiBoxWindowName, &this->configParams.edgeParams.recWidth, max_val_x, roiLaneCallback, &this->configParams.edgeParams);
	createTrackbar("Y1-Pos", this->configParams.edgeParams.roiBoxWindowName, &this->configParams.edgeParams.y1_roi, max_val_y, roiLaneCallback, &this->configParams.edgeParams);
	createTrackbar("RecHeight", this->configParams.edgeParams.roiBoxWindowName, &this->configParams.edgeParams.recHeight, max_val_y, roiLaneCallback, &this->configParams.edgeParams);

	edgeParametersP();
	waitKey();
	//destroyAllWindows();
	//destroyWindow(this->configParams.edgeParams.blurThreshWindowName);
}
void LineDetector::roiLaneCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);   // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);


	Mat img_ = edgeConfig->origImg.clone();
	Mat roiImg;
	if (edgeConfig->x1_roi + edgeConfig->recWidth >= img_.cols) {
		edgeConfig->recWidth = img_.cols - edgeConfig->x1_roi;
		setTrackbarPos("RecWidth", edgeConfig->roiBoxWindowName, edgeConfig->recWidth);
	}

	if (edgeConfig->y1_roi + edgeConfig->recHeight >= img_.rows) {
		edgeConfig->recHeight = img_.rows - edgeConfig->y1_roi;
		setTrackbarPos("RecHeight", edgeConfig->roiBoxWindowName, edgeConfig->recHeight);
	}

	Rect roi_box = Rect(edgeConfig->x1_roi, edgeConfig->y1_roi, edgeConfig->recWidth, edgeConfig->recHeight);
	edgeConfig->roi_Bbox = roi_box;
	roiImg = img_(roi_box);
	//cvtColor(img_(roi_box), roiImg, COLOR_BGR2GRAY);

	edgeConfig->roiImg = roiImg.clone();

	int img_num = 0;

	displayImg(roiImg, edgeConfig->roiBoxWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	edgeDetectCallback(0, edgeConfig);
}


void LineDetector::setCarDetectionROIBox() {
	namedWindow(this->configParams.edgeParams.roiBoxWindowName_car, WINDOW_AUTOSIZE);
	//moveWindow(this->configParams.edgeParams.roiBoxWindowName_car, 0, 0);
	size_t max_val_x = this->configParams.edgeParams.origImg.cols;
	size_t max_val_y = this->configParams.edgeParams.origImg.rows;

	createTrackbar("X1-Pos", this->configParams.edgeParams.roiBoxWindowName_car, &this->configParams.edgeParams.x1_roi_car, max_val_x, roiCarCallback, &this->configParams.edgeParams);
	createTrackbar("RecWidth", this->configParams.edgeParams.roiBoxWindowName_car, &this->configParams.edgeParams.recWidth_car, max_val_x, roiCarCallback, &this->configParams.edgeParams);
	createTrackbar("Y1-Pos", this->configParams.edgeParams.roiBoxWindowName_car, &this->configParams.edgeParams.y1_roi_car, max_val_y, roiCarCallback, &this->configParams.edgeParams);
	createTrackbar("RecHeight", this->configParams.edgeParams.roiBoxWindowName_car, &this->configParams.edgeParams.recHeight_car, max_val_y, roiCarCallback, &this->configParams.edgeParams);
};

void LineDetector::roiCarCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);   // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);
	//setTrackbarPos("X1-Pos", edgeConfig->roiBoxWindowName_car, edgeConfig->x1_roi_car);
	//setTrackbarPos("Y1-Pos", edgeConfig->roiBoxWindowName_car, edgeConfig->y1_roi_car);

	Mat img_ = edgeConfig->origImg.clone();
	Mat roiImg;
	if (edgeConfig->x1_roi_car + edgeConfig->recWidth_car >= img_.cols) {
		edgeConfig->recWidth_car = img_.cols - edgeConfig->x1_roi_car;

		setTrackbarPos("RecWidth", edgeConfig->roiBoxWindowName_car, edgeConfig->recWidth_car);
	}

	if (edgeConfig->y1_roi_car + edgeConfig->recHeight_car >= img_.rows) {
		edgeConfig->recHeight_car = img_.rows - edgeConfig->y1_roi_car;
		setTrackbarPos("RecHeight", edgeConfig->roiBoxWindowName_car, edgeConfig->recHeight_car);
	}

	Rect roi_box_car = Rect(edgeConfig->x1_roi_car, edgeConfig->y1_roi_car, edgeConfig->recWidth_car, edgeConfig->recHeight_car);
	edgeConfig->roi_Box_car = roi_box_car;
	roiImg = img_(roi_box_car);
	//cvtColor(img_(roi_box), roiImg, COLOR_BGR2GRAY);

	edgeConfig->roiImg_car = roiImg.clone();

	int img_num = -1;

	displayImg(roiImg, edgeConfig->roiBoxWindowName_car, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	waitKey();
	destroyAllWindows();
};


/* -------------------- Processing --------------------*/
void LineDetector::Pipeline() {


}

void LineDetector::processImg() {
	detectLanes();
}

void LineDetector::trackCars(){


}

void LineDetector::detectLanes() {
	Mat roi_img = this->configParams.edgeParams.currImg(this->configParams.edgeParams.roi_Bbox);
	int cannyKernelSize = this->configParams.edgeParams.cannyKernel + 3;
	cv::Mat cannyImg;

	// 1. Blur 
	// Blur:
	if (this->configParams.edgeParams.gauss_ksize > 0) {
		cvtColor(roi_img, cannyImg, COLOR_BGR2GRAY);
		blur(cannyImg, cannyImg, Size(this->configParams.edgeParams.gauss_ksize, this->configParams.edgeParams.gauss_ksize), Point(-1, -1));
	}
	// 2. Canny Edge Detection:
	if (!cannyImg.empty()) {
		Canny(cannyImg, cannyImg, this->configParams.edgeParams.lowThresh, this->configParams.edgeParams.highThresh, cannyKernelSize);
		this->configParams.edgeParams.cannyImg = cannyImg.clone();
	}

	// 3. Morphological operations
	int operation = this->configParams.edgeParams.morphTransformType + 1;
	Mat morphImg = cannyImg;

	// Morphological Operations A:
	if (operation > 3) {
		Mat str_element = getStructuringElement(this->configParams.edgeParams.morph_elem_shape, Size(2 * this->configParams.edgeParams.kernel_morph_size + 1, 2 * this->configParams.edgeParams.kernel_morph_size + 1), Point(this->configParams.edgeParams.kernel_morph_size, this->configParams.edgeParams.kernel_morph_size));
		operation -= 2;
		morphologyEx(morphImg, morphImg, operation, str_element);
	}
	else if (operation == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(this->configParams.edgeParams.kernel_morph_size + 1, this->configParams.edgeParams.kernel_morph_size + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
	}

	else if (operation == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(this->configParams.edgeParams.kernel_morph_size + 1, this->configParams.edgeParams.kernel_morph_size + 1), Point(this->configParams.edgeParams.kernel_morph_size, this->configParams.edgeParams.kernel_morph_size));
		dilate(morphImg, morphImg, element);
	}

	// Morphological Operations B:
	operation = this->configParams.edgeParams.morphTransformType_ + 1;

	if (operation > 3) {
		Mat str_element = getStructuringElement(this->configParams.edgeParams.morph_elem_shape, Size(2 * this->configParams.edgeParams.kernel_morph_size_ + 1, 2 * this->configParams.edgeParams.kernel_morph_size_ + 1), Point(this->configParams.edgeParams.kernel_morph_size_, this->configParams.edgeParams.kernel_morph_size_));
		operation -= 2;
		morphologyEx(morphImg, morphImg, operation, str_element);
	}
	else if (operation == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(this->configParams.edgeParams.kernel_morph_size_ + 1, this->configParams.edgeParams.kernel_morph_size_ + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
	}

	else if (operation == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(this->configParams.edgeParams.kernel_morph_size_ + 1, this->configParams.edgeParams.kernel_morph_size_ + 1), Point(this->configParams.edgeParams.kernel_morph_size_, this->configParams.edgeParams.kernel_morph_size_));
		dilate(morphImg, morphImg, element);
	}

	if (morphImg.empty()) {
		morphImg = this->configParams.edgeParams.origImg;
	}

	this->configParams.edgeParams.morphImg2 = morphImg;
	Mat houghImg = this->configParams.edgeParams.morphImg2.clone();

	// 4. Determine hough lines:
	vector<Vec4i> lines;
	cv::HoughLinesP(houghImg, lines, 1, CV_PI / 180, this->configParams.edgeParams.minVotes, this->configParams.edgeParams.minLineLength, this->configParams.edgeParams.maxLineGap);

	this->configParams.edgeParams.edgeLines = lines;
	bool laneDetect = true;
	drawLines(&this->configParams.edgeParams, this->configParams.edgeParams.currImg, lines, laneDetect);
}



/* -------------------- Helper functions --------------------*/
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
void LineDetector::drawLines(EdgeConfig* edgeParams, cv::Mat& img, std::vector<cv::Vec4i> lines, bool laneDetection) {

	int x_offset = edgeParams->x1_roi;
	int y_offset = edgeParams->y1_roi;
	float lane_1_m = 0.;
	float lane_2_m = 0.;
	int count_1 = 0;
	float lane_1_yc = 0.;
	float lane_2_yc = 0.;
	int count_2 = 0;
	Point roi_lane1_pt1, roi_lane1_pt2, roi_lane2_pt1, roi_lane2_pt2;
	for (auto it = lines.begin(); it != lines.end(); ++it) {
		Point a, b;
		auto line = *it;
		int x = line[0] + line[1] + line[2] + line[3];

		if (x != 0) {
			a.x = line[0] + x_offset;
			a.y = line[1] + y_offset;
			b.x = line[2] + x_offset;
			b.y = line[3] + y_offset;
			if (laneDetection) {
				float m = float(b.y - a.y) / float(b.x - a.x);
				float yc = b.y - m * a.y;
				if (m >= 0.43 & m <= 0.58) {
					lane_1_m += m;
					lane_1_yc += yc;
					roi_lane1_pt1.x += a.x;
					roi_lane1_pt1.y += a.y;
					roi_lane1_pt2.x += b.x;
					roi_lane1_pt2.y += b.y;
					count_1 += 1;
				}

				else if (m >= -0.75  & m <= -0.62) {
					lane_2_m += m;
					lane_2_yc += yc;
					roi_lane2_pt1.x += a.x;
					roi_lane2_pt1.y += a.y;
					roi_lane2_pt2.x += b.x;
					roi_lane2_pt2.y += b.y;
					count_2 += 1;
				}
			}
			else {
				cv::line(img, a, b, Scalar(0, 0, 255), edgeParams->lineThickness, LINE_AA);
			}
		}
	}
	if (laneDetection) {
		float avgMLane1 = float(lane_1_m) / float(count_1);
		float avgMLane2 = float(lane_2_m) / float(count_2);
		float avgYc1 = float(lane_1_yc) / float(count_1);
		float avgYc2 = float(lane_2_yc) / float(count_2);

		roi_lane1_pt1.x = float(roi_lane1_pt1.x) / float(count_1);
		roi_lane1_pt1.y = float(roi_lane1_pt1.y) / float(count_1);
		roi_lane1_pt2.x = float(roi_lane1_pt2.x) / float(count_1);
		roi_lane1_pt2.y = float(roi_lane1_pt2.y) / float(count_1);

		roi_lane2_pt1.x = float(roi_lane2_pt1.x) / float(count_2);
		roi_lane2_pt1.y = float(roi_lane2_pt1.y) / float(count_2);
		roi_lane2_pt2.x = float(roi_lane2_pt2.x) / float(count_2);
		roi_lane2_pt2.y = float(roi_lane2_pt2.y) / float(count_2);

		Point dashboardLane1, dashboardLane2, line1End, line2End;
		dashboardLane1.y = img.rows;
		dashboardLane2.y = img.rows;

		if (roi_lane1_pt1.y < roi_lane1_pt2.y) {
			line1End = roi_lane1_pt1;
			dashboardLane1.x = (img.rows - roi_lane1_pt1.y) / avgMLane1 + roi_lane1_pt1.x;
		}

		else {
			line1End = roi_lane1_pt2;
			dashboardLane1.x = (img.rows - roi_lane1_pt2.y) / avgMLane1 + roi_lane1_pt2.x;
		}

		if (roi_lane2_pt1.y < roi_lane2_pt2.y) {
			line2End = roi_lane2_pt1;
			dashboardLane2.x = (img.rows - roi_lane2_pt1.y) / avgMLane2 + roi_lane2_pt1.x;
		}

		else {
			line2End = roi_lane2_pt2;
			dashboardLane2.x = (img.rows - roi_lane2_pt2.y) / avgMLane2 + roi_lane2_pt2.x;
		}

		if (count_1 > 0) {
			cv::arrowedLine(img, dashboardLane1, line1End, Scalar(0, 255, 0), edgeParams->lineThickness, LINE_AA);
			edgeParams->line1_pt1 = line1End;
			edgeParams->line1_pt2 = dashboardLane1;
		}
		else {
			cv::arrowedLine(img, edgeParams->line1_pt1, edgeParams->line1_pt2, Scalar(0, 0, 255), edgeParams->lineThickness, LINE_AA);
		}

		if (count_2 > 0) {
			cv::arrowedLine(img, dashboardLane2, line2End, Scalar(0, 255, 0), edgeParams->lineThickness, LINE_AA);
			edgeParams->line2_pt1 = line2End;
			edgeParams->line2_pt2 = dashboardLane2;
		}
		else {
			cv::arrowedLine(img, edgeParams->line2_pt1, edgeParams->line2_pt2, Scalar(0, 0, 255), edgeParams->lineThickness, LINE_AA);
		}
	}






}
void LineDetector::drawCircles(EdgeConfig* edgeParams, cv::Mat& img, const std::vector<cv::Vec3f>& circles) {

	for (size_t i = 0; i < circles.size(); i++)
	{
		int x_offset = edgeParams->x1_roi;
		int y_offset = edgeParams->y1_roi;

		int radius = cvRound(circles[i][2]);
		Point centre(cvRound(circles[i][0]) + x_offset, cvRound(circles[i][1]) + y_offset);

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
//void LineDetector::calcImgDims(EdgeConfig * edgeParams, cv::Mat & img) {};


/* -------------------- Multithreading -------------------- */

void LineDetector::processImg_thread(LineDetector& ld, atomic<bool>& stopThreads) {

	int cannyKernelSize = ld.configParams.edgeParams.cannyKernel + 3;
	cv::Mat cannyImg;

	while (ld.configParams.edgeParams.continueProcessing) {

		if (ld.configParams.edgeParams.newImgAvailable) {
			ld.configParams.edgeParams.linesDrawn = false;
			Mat roi_img = ld.configParams.edgeParams.currImg(ld.configParams.edgeParams.roi_Bbox);
			Mat curr_img_copy = ld.configParams.edgeParams.currImg.clone();
			// 1. Blur 
			// Blur:
			if (ld.configParams.edgeParams.gauss_ksize > 0) {
				cvtColor(roi_img, cannyImg, COLOR_BGR2GRAY);
				blur(cannyImg, cannyImg, Size(ld.configParams.edgeParams.gauss_ksize, ld.configParams.edgeParams.gauss_ksize), Point(-1, -1));
			}
			// 2. Canny Edge Detection:
			if (!cannyImg.empty()) {
				Canny(cannyImg, cannyImg, ld.configParams.edgeParams.lowThresh, ld.configParams.edgeParams.highThresh, cannyKernelSize);
				ld.configParams.edgeParams.cannyImg = cannyImg.clone();
			}

			// 3. Morphological operations
			int operation = ld.configParams.edgeParams.morphTransformType + 1;
			Mat morphImg = cannyImg;

			// Morphological Operations A:
			if (operation > 3) {
				Mat str_element = getStructuringElement(ld.configParams.edgeParams.morph_elem_shape, Size(2 * ld.configParams.edgeParams.kernel_morph_size + 1, 2 * ld.configParams.edgeParams.kernel_morph_size + 1), Point(ld.configParams.edgeParams.kernel_morph_size, ld.configParams.edgeParams.kernel_morph_size));
				operation -= 2;
				morphologyEx(morphImg, morphImg, operation, str_element);
			}
			else if (operation == 1) {
				Mat element = getStructuringElement(MORPH_CROSS, Size(ld.configParams.edgeParams.kernel_morph_size + 1, ld.configParams.edgeParams.kernel_morph_size + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
				erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
			}

			else if (operation == 2) {
				Mat element = getStructuringElement(MORPH_RECT, Size(ld.configParams.edgeParams.kernel_morph_size + 1, ld.configParams.edgeParams.kernel_morph_size + 1), Point(ld.configParams.edgeParams.kernel_morph_size, ld.configParams.edgeParams.kernel_morph_size));
				dilate(morphImg, morphImg, element);
			}

			// Morphological Operations B:
			operation = ld.configParams.edgeParams.morphTransformType_ + 1;

			if (operation > 3) {
				Mat str_element = getStructuringElement(ld.configParams.edgeParams.morph_elem_shape, Size(2 * ld.configParams.edgeParams.kernel_morph_size_ + 1, 2 * ld.configParams.edgeParams.kernel_morph_size_ + 1), Point(ld.configParams.edgeParams.kernel_morph_size_, ld.configParams.edgeParams.kernel_morph_size_));
				operation -= 2;
				morphologyEx(morphImg, morphImg, operation, str_element);
			}
			else if (operation == 1) {
				Mat element = getStructuringElement(MORPH_CROSS, Size(ld.configParams.edgeParams.kernel_morph_size_ + 1, ld.configParams.edgeParams.kernel_morph_size_ + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
				erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
			}

			else if (operation == 2) {
				Mat element = getStructuringElement(MORPH_RECT, Size(ld.configParams.edgeParams.kernel_morph_size_ + 1, ld.configParams.edgeParams.kernel_morph_size_ + 1), Point(ld.configParams.edgeParams.kernel_morph_size_, ld.configParams.edgeParams.kernel_morph_size_));
				dilate(morphImg, morphImg, element);
			}

			if (morphImg.empty()) {
				morphImg = ld.configParams.edgeParams.origImg;
			}

			ld.configParams.edgeParams.morphImg2 = morphImg;
			Mat houghImg = ld.configParams.edgeParams.morphImg2.clone();

			// 4. Determine hough lines:
			vector<Vec4i> lines;
			try {
				cv::HoughLinesP(houghImg, lines, 1, CV_PI / 180, ld.configParams.edgeParams.minVotes, ld.configParams.edgeParams.minLineLength, ld.configParams.edgeParams.maxLineGap);
			}
			catch (cv::Exception& e) {
				int val = 0;
			}
			ld.configParams.edgeParams.edgeLines = lines;
			bool laneDetect = true;
			
			drawLines(&ld.configParams.edgeParams, curr_img_copy, lines, laneDetect);
			//drawLines(&ld.configParams.edgeParams, ld.configParams.edgeParams.currImg, lines, laneDetect);
			ld.configParams.edgeParams.currImg = curr_img_copy;
			ld.configParams.edgeParams.linesDrawn = true;
			// 5. Confirm image_i processed
			ld.configParams.edgeParams.newImgAvailable = false;
		}
		else {
			this_thread::sleep_for(chrono::milliseconds(10));
		}
		if (stopThreads) {
			return;
		}
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

