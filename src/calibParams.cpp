#include <calibParams.h>
using namespace cv;
using namespace std;
/* -------------------- Preprocessing --------------------*/

void CalibParams::houghCParameters() {

	namedWindow(this->m_configParams.houghCWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->m_configParams.houghCWindowName, int(2 * this->m_configParams.screenWidth / 3), int(this->m_configParams.screenHeight / 2));
	resizeWindow(this->m_configParams.houghCWindowName, int(m_configParams.newCols), int(m_configParams.newRows));

	createTrackbar("minRadius", this->m_configParams.houghCWindowName, &this->m_configParams.minRadius, int(this->m_configParams.minRadius * 2.5), houghC_Callback, &this->m_configParams);
	createTrackbar("maxRadius", this->m_configParams.houghCWindowName, &this->m_configParams.maxRadius, int(this->m_configParams.maxRadius * 2.5), houghC_Callback, &this->m_configParams);
	createTrackbar("minDistBtw", this->m_configParams.houghCWindowName, &this->m_configParams.minDistBtwCenters, int(this->m_configParams.minDistBtwCenters * 2.5), houghC_Callback, &this->m_configParams);
	createTrackbar("cntrThresh", this->m_configParams.houghCWindowName, &this->m_configParams.centreThresh, int(this->m_configParams.centreThresh * 2.5), houghC_Callback, &this->m_configParams);

	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
	//destroyWindow(this->configParams.houghCWindowName);
};
void CalibParams::houghC_Callback(int, void* userdata) {

	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	Mat houghImg = configP->morphImg2.clone();

	// 2. Determine elliptical shapes:
	vector<Vec3f> circles;
	Mat houghTransform = configP->origImg.clone();
	// int minRadiusOffset = configP->minRadius + 
	try {
		cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, configP->dp, configP->minDistBtwCenters, configP->highThresh, configP->centreThresh, configP->minRadius, configP->maxRadius);
	}
	catch (cv::Exception) {
		cout << "Please set roi-box first." << endl;
	
	int img_num = 5;
	//drawLines(configP, houghTransform, lines);
	displayImg(houghTransform, configP->houghCWindowName, configP->screenWidth, configP->screenHeight, img_num);
	}
}

void CalibParams::houghLParametersP() {

	namedWindow(this->m_configParams.houghWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->m_configParams.houghWindowName, int(2 * this->m_configParams.screenWidth / 3), 0);
	resizeWindow(this->m_configParams.houghWindowName, int(m_configParams.newCols), int(m_configParams.newRows));

	createTrackbar("minVotes", this->m_configParams.houghWindowName, &this->m_configParams.minVotes, this->m_configParams.minVotesLim, houghLPCallback, &this->m_configParams);
	createTrackbar("minLength", this->m_configParams.houghWindowName, &this->m_configParams.minLineLength, this->m_configParams.minLineLengthLimit, houghLPCallback, &this->m_configParams);
	createTrackbar("maxGap", this->m_configParams.houghWindowName, &this->m_configParams.maxLineGap, this->m_configParams.maxLineGapLimit, houghLPCallback, &this->m_configParams);

	houghCParameters();
	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
	//destroyWindow(this->configParams.houghWindowName);
};
void CalibParams::houghLPCallback(int, void* userdata) {

	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	Mat houghImg = configP->morphImg2.clone();

	// 1. Determine hough lines:
	vector<Vec4i> lines;
	try {
		HoughLinesP(houghImg, lines, 1, CV_PI / 180, configP->minVotes, configP->minLineLength, configP->maxLineGap);

		configP->edgeLines = lines;
		Mat houghTransform = configP->origImg.clone();
		/*
		// 2. Determine elliptical shapes:
		vector<Vec3f> circles;
		cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, configP->dp, configP->minDistBtwCenters, configP->highThresh, configP->centreThresh, configP->minRadius, configP->maxRadius);
		*/
		int img_num = 4;
		drawLines(configP, houghTransform, lines);
		//drawCircles(configP, houghTransform, circles);
		displayImg(houghTransform, configP->houghWindowName, configP->screenWidth, configP->screenHeight, img_num);
		houghC_Callback(0, configP);
	}
	catch (cv::Exception) {
		cout << "Please set roi-box first." << endl;
	}
}


void CalibParams::morphParametersP2() {

	namedWindow(this->m_configParams.morphWindowName2, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->m_configParams.morphWindowName2, int(this->m_configParams.screenWidth / 3), int(this->m_configParams.screenHeight / 2));
	resizeWindow(this->m_configParams.morphWindowName2, int(m_configParams.newCols), int(m_configParams.newRows));

	createTrackbar("MorphType", this->m_configParams.morphWindowName2, &this->m_configParams.morphTransformType_, 8, morphCallback2, &this->m_configParams);
	createTrackbar("KernelSize", this->m_configParams.morphWindowName2, &this->m_configParams.kernel_morph_size_, 5, morphCallback2, &this->m_configParams);
	//createTrackbar("MorphShape", this->configParams.morphWindowName, &this->configParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams);

	houghLParametersP();
	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
}
void CalibParams::morphCallback2(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	int operation = configP->morphTransformType_ + 1;

	Mat morphImg2 = configP->morphImg.clone();
	try {
		// Morphological Operations:
		if (operation > 3) {
			Mat str_element = getStructuringElement(configP->morph_elem_shape, Size(2 * configP->kernel_morph_size_ + 1, 2 * configP->kernel_morph_size_ + 1), Point(configP->kernel_morph_size_, configP->kernel_morph_size_));
			operation -= 2;
			morphologyEx(morphImg2, morphImg2, operation, str_element);
		}
		else if (operation == 1) {
			//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
			//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
			//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

			Mat element = getStructuringElement(MORPH_CROSS, Size(configP->kernel_morph_size_ + 1, configP->kernel_morph_size_ + 1));//, Point(configP->kernel_morph_size, configP->kernel_morph_size));
			erode(morphImg2, morphImg2, element, Point(-1, -1), 2, 1, 1);
		}

		else if (operation == 2) {
			//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
			//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
			//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

			Mat element = getStructuringElement(MORPH_RECT, Size(configP->kernel_morph_size_ + 1, configP->kernel_morph_size_ + 1), Point(configP->kernel_morph_size_, configP->kernel_morph_size_));
			dilate(morphImg2, morphImg2, element);
		}

		if (morphImg2.empty()) {
			morphImg2 = configP->origImg;
		}
		configP->morphImg2 = morphImg2;

		int img_num = 3;
		displayImg(morphImg2, configP->morphWindowName2, configP->screenWidth, configP->screenHeight, img_num);
		houghLPCallback(0, configP);
	}
	catch (cv::Exception) {
		cout << "Please set roi-box first." << endl;
	}
}

void CalibParams::morphParametersP() {

	namedWindow(this->m_configParams.morphWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->m_configParams.morphWindowName, int(this->m_configParams.screenWidth / 3), 0);
	resizeWindow(this->m_configParams.morphWindowName, int(m_configParams.newCols), int(m_configParams.newRows));

	createTrackbar("MorphType", this->m_configParams.morphWindowName, &this->m_configParams.morphTransformType, 8, morphCallback, &this->m_configParams);
	createTrackbar("KernelSize", this->m_configParams.morphWindowName, &this->m_configParams.kernel_morph_size, 5, morphCallback, &this->m_configParams);
	//createTrackbar("MorphShape", this->m_configParams.morphWindowName, &this->m_configParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams);

	morphParametersP2();
	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
}
void CalibParams::morphCallback(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	int operation = configP->morphTransformType + 1;

	Mat morphImg = configP->cannyImg.clone();
	try {
		// Morphological Operations:
		if (operation > 3) {
			Mat str_element = getStructuringElement(configP->morph_elem_shape, Size(2 * configP->kernel_morph_size + 1, 2 * configP->kernel_morph_size + 1), Point(configP->kernel_morph_size, configP->kernel_morph_size));
			operation -= 2;
			morphologyEx(morphImg, morphImg, operation, str_element);
		}
		else if (operation == 1) {
			//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
			//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
			//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

			Mat element = getStructuringElement(MORPH_CROSS, Size(configP->kernel_morph_size + 1, configP->kernel_morph_size + 1));//, Point(configP->kernel_morph_size, configP->kernel_morph_size));
			erode(morphImg, morphImg, element, Point(-1, -1), 2, 1, 1);
		}

		else if (operation == 2) {
			//if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
			//else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
			//else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

			Mat element = getStructuringElement(MORPH_RECT, Size(configP->kernel_morph_size + 1, configP->kernel_morph_size + 1), Point(configP->kernel_morph_size, configP->kernel_morph_size));
			dilate(morphImg, morphImg, element);
		}

		if (morphImg.empty()) {
			morphImg = configP->origImg;
		}
		configP->morphImg = morphImg;

		int img_num = 2;
		displayImg(morphImg, configP->morphWindowName, configP->screenWidth, configP->screenHeight, img_num);
		morphCallback2(0, configP);
	}
	catch (cv::Exception) {
		cout << "Please set roi-box first." << endl;
	}
}

void CalibParams::edgeParametersP() {

	namedWindow(this->m_configParams.edgeWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->m_configParams.edgeWindowName, int(m_configParams.newCols), int(m_configParams.newRows));
	moveWindow(this->m_configParams.edgeWindowName, 0, int(this->m_configParams.screenHeight / 2));

	createTrackbar("BlurLevel", this->m_configParams.edgeWindowName, &this->m_configParams.gauss_ksize, 15, edgeDetectCallback, &this->m_configParams);
	createTrackbar("CannyLow", this->m_configParams.edgeWindowName, &this->m_configParams.lowThresh, 100, edgeDetectCallback, &this->m_configParams);
	createTrackbar("CannyHigh", this->m_configParams.edgeWindowName, &this->m_configParams.highThresh, 150, edgeDetectCallback, &this->m_configParams);

	morphParametersP();
	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
};
void CalibParams::edgeDetectCallback(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	int operation = configP->morphTransformType + 1;
	int cannyKernelSize = configP->cannyKernel + 3;
	cv::Mat cannyImg;
	Mat img_ = configP->roiImg.clone();
	try {
		// Blur:
		if (configP->gauss_ksize > 0) {
			cvtColor(img_, cannyImg, COLOR_BGR2GRAY);
			blur(cannyImg, cannyImg, Size(configP->gauss_ksize, configP->gauss_ksize), Point(-1, -1));
		}
		// Edge Detection:
		if (!cannyImg.empty()) {
			if (configP->highThresh == 0) {
				configP->highThresh++;
			}
			Canny(cannyImg, cannyImg, configP->lowThresh, configP->highThresh, cannyKernelSize);
			configP->cannyImg = cannyImg.clone();
			int img_num = 1;
			displayImg(cannyImg, configP->edgeWindowName, configP->screenWidth, configP->screenHeight, img_num);
			morphCallback(0, configP);
		}

		if (0) {
			if (img_.empty()) {
				img_ = configP->origImg;
				cannyImg = img_.clone();
			}
			else {

				// Edge Detection:
				Canny(img_, cannyImg, configP->lowThresh, configP->highThresh, cannyKernelSize);
				configP->cannyImg = cannyImg.clone();

			}
		}
	}
	catch (cv::Exception) {
		cout << "Please set roi-box first." << endl;
	}
}

void CalibParams::blurThreshParametersP() {

	namedWindow(this->m_configParams.blurThreshWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->m_configParams.blurThreshWindowName, int(m_configParams.newCols), int(m_configParams.newRows));
	moveWindow(this->m_configParams.blurThreshWindowName, 0, 0);

	createTrackbar("BlurLevel", this->m_configParams.blurThreshWindowName, &this->m_configParams.gauss_ksize, 10, blurThreshCallback, &this->m_configParams);
	//createTrackbar("TheshLevel", this->configParams.BlurThreshWindowName, &this->configParams.thresBin, 255, blurThreshCallback, &this->configParams);

	edgeParametersP();
	waitKey();
	if (!this->m_configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->m_configParams.terminateSetup = true;
	}
}
void CalibParams::blurThreshCallback(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);
	int operation = configP->morphTransformType + 1;
	int cannyKernelSize = configP->cannyKernel + 3;
	cv::Mat blurThreshImg = configP->origImg.clone();
	Mat img_ = configP->origImg.clone();

	cvtColor(img_, blurThreshImg, COLOR_BGR2GRAY);

	// Blur:
	if (configP->gauss_ksize > 0) {
		blur(blurThreshImg, blurThreshImg, Size(configP->gauss_ksize, configP->gauss_ksize), Point(-1, -1));
	}
	// Thresholding

	//threshold(blurThreshImg, blurThreshImg, configP->thresBin, 255, 0);
	//cvtColor(blurThreshImg, blurThreshImg, COLOR_GRAY2BGR);

	configP->blurThreshImg = blurThreshImg.clone();

	int img_num = 0;

	displayImg(blurThreshImg, configP->blurThreshWindowName, configP->screenWidth, configP->screenHeight, img_num);
	edgeDetectCallback(0, configP);
}


void CalibParams::getDesktopResolution() {

	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	m_configParams.screenWidth = desktop.right;
	m_configParams.screenHeight = desktop.bottom;
}


void CalibParams::setup(CalibParams& cb_, cv::Mat& img)
{
	cb_.getDesktopResolution();
	int maxHeight = int(cb_.m_configParams.screenHeight / 2);
	int maxWidth = int(cb_.m_configParams.screenWidth / 3);

	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;
	}

	else {
		new_rows = maxHeight;
	}

	cb_.m_configParams.newCols = new_cols;
	cb_.m_configParams.newRows = new_rows;
	//cb_.m_configParams.recHeight = new_rows;
	//cb_.m_configParams.recWidth = new_cols;

	cv::Mat m;
	const std::string winName = "Background";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	m.create(int(cb_.m_configParams.screenHeight * 0.95), cb_.m_configParams.screenWidth, CV_32FC3);
	m.setTo(cv::Scalar(255, 255, 255));
	moveWindow(winName, 0, 0);
	cv::imshow(winName, m);

	cb_.m_configParams.origImg = img.clone();
	cb_.setLane_ROIBox();
	cb_.setCarDetectionROIBox();

}

void CalibParams::setLane_ROIBox() {
	namedWindow(this->m_configParams.roiBoxWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->m_configParams.roiBoxWindowName, int(m_configParams.newCols), int(m_configParams.newRows));
	moveWindow(this->m_configParams.roiBoxWindowName, 0, 0);
	size_t max_val_x = this->m_configParams.origImg.cols;
	size_t max_val_y = this->m_configParams.origImg.rows;

	createTrackbar("X1-Pos", this->m_configParams.roiBoxWindowName, &this->m_configParams.x1_roi, max_val_x, roiLaneCallback, &this->m_configParams);
	createTrackbar("RecWidth", this->m_configParams.roiBoxWindowName, &this->m_configParams.recWidth, max_val_x, roiLaneCallback, &this->m_configParams);
	createTrackbar("Y1-Pos", this->m_configParams.roiBoxWindowName, &this->m_configParams.y1_roi, max_val_y, roiLaneCallback, &this->m_configParams);
	createTrackbar("RecHeight", this->m_configParams.roiBoxWindowName, &this->m_configParams.recHeight, max_val_y, roiLaneCallback, &this->m_configParams);

	edgeParametersP();
	waitKey();
	//destroyAllWindows();
	//destroyWindow(this->configParams.blurThreshWindowName);
}
void CalibParams::roiLaneCallback(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);  // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);


	Mat img_ = configP->origImg.clone();
	Mat roiImg;
	if (configP->x1_roi + configP->recWidth >= img_.cols) {
		configP->recWidth = img_.cols - configP->x1_roi;
		setTrackbarPos("RecWidth", configP->roiBoxWindowName, configP->recWidth);
	}

	if (configP->y1_roi + configP->recHeight >= img_.rows) {
		configP->recHeight = img_.rows - configP->y1_roi;
		setTrackbarPos("RecHeight", configP->roiBoxWindowName, configP->recHeight);
	}

	Rect roi_box = Rect(configP->x1_roi, configP->y1_roi, configP->recWidth, configP->recHeight);
	configP->roi_Bbox = roi_box;
	roiImg = img_(roi_box);
	//cvtColor(img_(roi_box), roiImg, COLOR_BGR2GRAY);

	configP->roiImg = roiImg.clone();

	int img_num = 0;

	displayImg(roiImg, configP->roiBoxWindowName, configP->screenWidth, configP->screenHeight, img_num);
	edgeDetectCallback(0, configP);
}


void CalibParams::setCarDetectionROIBox() {
	namedWindow(this->m_configParams.roiBoxWindowName_car, WINDOW_AUTOSIZE);
	//moveWindow(this->m_configParams.roiBoxWindowName_car, 0, 0);
	size_t max_val_x = this->m_configParams.origImg.cols;
	size_t max_val_y = this->m_configParams.origImg.rows;

	createTrackbar("X1-Pos", this->m_configParams.roiBoxWindowName_car, &this->m_configParams.x1_roi_car, max_val_x, roiCarCallback, &this->m_configParams);
	createTrackbar("RecWidth", this->m_configParams.roiBoxWindowName_car, &this->m_configParams.recWidth_car, max_val_x, roiCarCallback, &this->m_configParams);
	createTrackbar("Y1-Pos", this->m_configParams.roiBoxWindowName_car, &this->m_configParams.y1_roi_car, max_val_y, roiCarCallback, &this->m_configParams);
	createTrackbar("RecHeight", this->m_configParams.roiBoxWindowName_car, &this->m_configParams.recHeight_car, max_val_y, roiCarCallback, &this->m_configParams);
	waitKey();
	destroyAllWindows();
};

void CalibParams::roiCarCallback(int, void* userdata) {
	ConfigParams* configP = reinterpret_cast<ConfigParams*>(userdata);   // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);
	//setTrackbarPos("X1-Pos", configP->roiBoxWindowName_car, configP->x1_roi_car);
	//setTrackbarPos("Y1-Pos", configP->roiBoxWindowName_car, configP->y1_roi_car);

	Mat img_ = configP->origImg.clone();
	Mat roiImg;
	if (configP->x1_roi_car + configP->recWidth_car >= img_.cols) {
		configP->recWidth_car = img_.cols - configP->x1_roi_car;

		setTrackbarPos("RecWidth", configP->roiBoxWindowName_car, configP->recWidth_car);
	}

	if (configP->y1_roi_car + configP->recHeight_car >= img_.rows) {
		configP->recHeight_car = img_.rows - configP->y1_roi_car;
		setTrackbarPos("RecHeight", configP->roiBoxWindowName_car, configP->recHeight_car);
	}

	Rect roi_box_car = Rect(configP->x1_roi_car, configP->y1_roi_car, configP->recWidth_car, configP->recHeight_car);
	configP->roi_Box_car = roi_box_car;
	roiImg = img_(roi_box_car);
	//cvtColor(img_(roi_box), roiImg, COLOR_BGR2GRAY);

	configP->roiImg_car = roiImg.clone();

	int img_num = -1;

	displayImg(roiImg, configP->roiBoxWindowName_car, configP->screenWidth, configP->screenHeight, img_num);

};

/* -------------------- Helper functions --------------------*/
void CalibParams::displayImg(const Mat& img, const std::string& title, const int& screenWidth, const int& screenHeight, const int& img_num) {

	//double img_width = img.cols;  //obtain size of image and halve it in order to fit 
	//double img_height = img.rows;
	try {
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
	catch (cv::Exception) {

		cout << "Defined window out of bounds." << endl;	
	}
}

void CalibParams::drawLines(ConfigParams* configParams, cv::Mat& img, const std::vector<cv::Vec4i>& lines, bool laneDetection) {

	int x_offset = configParams->x1_roi;
	int y_offset = configParams->y1_roi;
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

				else if (m >= -0.75 & m <= -0.62) {
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
				cv::line(img, a, b, Scalar(0, 0, 255), configParams->lineThickness, LINE_AA);
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
			cv::arrowedLine(img, dashboardLane1, line1End, Scalar(0, 255, 0), configParams->lineThickness, LINE_AA);
			configParams->line1_pt1 = line1End;
			configParams->line1_pt2 = dashboardLane1;
		}
		else {
			cv::arrowedLine(img, configParams->line1_pt1, configParams->line1_pt2, Scalar(0, 0, 255), configParams->lineThickness, LINE_AA);
		}

		if (count_2 > 0) {
			cv::arrowedLine(img, dashboardLane2, line2End, Scalar(0, 255, 0), configParams->lineThickness, LINE_AA);
			configParams->line2_pt1 = line2End;
			configParams->line2_pt2 = dashboardLane2;
		}
		else {
			cv::arrowedLine(img, configParams->line2_pt1, configParams->line2_pt2, Scalar(0, 0, 255), configParams->lineThickness, LINE_AA);
		}
	}






}

void CalibParams::updateParams()
{
	m_houghVar.minVotes = m_configParams.minVotes;
	m_houghVar.minLineLength = m_configParams.minLineLength;
	m_houghVar.maxLineGap = m_configParams.maxLineGap;
	m_houghVar.lineThickness = m_configParams.lineThickness;

	m_preprocessVar.cannyKernelSize = m_configParams.cannyKernel;
	m_preprocessVar.cannyHighTresh = m_configParams.highThresh;
	m_preprocessVar.cannyLowThresh = m_configParams.lowThresh;
	m_preprocessVar.gaussKernelSize = m_configParams.gauss_ksize;
	m_preprocessVar.morphElemShape = m_configParams.morph_elem_shape;
	m_preprocessVar.morphTransformType1 = m_configParams.morphTransformType;
	m_preprocessVar.morphKernelSize1 = m_configParams.kernel_morph_size;
	m_preprocessVar.morphTransformType2 = m_configParams.morphTransformType_;
	m_preprocessVar.morphKernelSize2 = m_configParams.kernel_morph_size_;

}

LineDetector::preprocessParams CalibParams::getPreprocessParams() const
{
	return m_preprocessVar;
}

LineDetector::houghParams CalibParams::getHoughParams() const
{
	return m_houghVar;
}











