#include <calibParams.h>
using namespace cv;
using namespace std;
/* -------------------- Preprocessing --------------------*/



void CalibParams::houghCParameters() {

	namedWindow(this->configParams.houghCWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.houghCWindowName, int(2 * this->configParams.screenWidth / 3), int(this->configParams.screenHeight / 2));
	resizeWindow(this->configParams.houghCWindowName, int(configParams.newCols), int(configParams.newRows));

	createTrackbar("minRadius", this->configParams.houghCWindowName, &this->configParams.minRadius, int(this->configParams.minRadius * 2.5), houghC_Callback, &this->configParams);
	createTrackbar("maxRadius", this->configParams.houghCWindowName, &this->configParams.maxRadius, int(this->configParams.maxRadius * 2.5), houghC_Callback, &this->configParams);
	createTrackbar("minDistBtw", this->configParams.houghCWindowName, &this->configParams.minDistBtwCenters, int(this->configParams.minDistBtwCenters * 2.5), houghC_Callback, &this->configParams);
	createTrackbar("cntrThresh", this->configParams.houghCWindowName, &this->configParams.centreThresh, int(this->configParams.centreThresh * 2.5), houghC_Callback, &this->configParams);

	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
	//destroyWindow(this->configParams.houghCWindowName);
};
void CalibParams::houghC_Callback(int, void *userdata) {

	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
	Mat houghImg = configP->morphImg2.clone();

	// 2. Determine elliptical shapes:
	vector<Vec3f> circles;
	Mat houghTransform = configP->origImg.clone();
	// int minRadiusOffset = configP->minRadius + 
	cv::HoughCircles(houghImg, circles, HOUGH_GRADIENT, configP->dp, configP->minDistBtwCenters, configP->highThresh, configP->centreThresh, configP->minRadius, configP->maxRadius);

	int img_num = 5;
	//drawLines(configP, houghTransform, lines);
	displayImg(houghTransform, configP->houghCWindowName, configP->screenWidth, configP->screenHeight, img_num);
}

void CalibParams::houghLParametersP() {

	namedWindow(this->configParams.houghWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.houghWindowName, int(2 * this->configParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.houghWindowName, int(configParams.newCols), int(configParams.newRows));

	createTrackbar("minVotes", this->configParams.houghWindowName, &this->configParams.minVotes, this->configParams.minVotesLim, houghLPCallback, &this->configParams);
	createTrackbar("minLength", this->configParams.houghWindowName, &this->configParams.minLineLength, this->configParams.minLineLengthLimit, houghLPCallback, &this->configParams);
	createTrackbar("maxGap", this->configParams.houghWindowName, &this->configParams.maxLineGap, this->configParams.maxLineGapLimit, houghLPCallback, &this->configParams);

	houghCParameters();
	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
	//destroyWindow(this->configParams.houghWindowName);
};
void CalibParams::houghLPCallback(int, void *userdata) {

	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
	Mat houghImg = configP->morphImg2.clone();

	// 1. Determine hough lines:
	vector<Vec4i> lines;
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

void CalibParams::morphParametersP2() {

	namedWindow(this->configParams.morphWindowName2, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->configParams.morphWindowName2, int(this->configParams.screenWidth / 3), int(this->configParams.screenHeight / 2));
	resizeWindow(this->configParams.morphWindowName2, int(configParams.newCols), int(configParams.newRows));

	createTrackbar("MorphType", this->configParams.morphWindowName2, &this->configParams.morphTransformType_, 8, morphCallback2, &this->configParams);
	createTrackbar("KernelSize", this->configParams.morphWindowName2, &this->configParams.kernel_morph_size_, 5, morphCallback2, &this->configParams);
	//createTrackbar("MorphShape", this->configParams.morphWindowName, &this->configParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams);

	houghLParametersP();
	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
}
void CalibParams::morphCallback2(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
	int operation = configP->morphTransformType_ + 1;

	Mat morphImg2 = configP->morphImg.clone();

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

void CalibParams::morphParametersP() {

	namedWindow(this->configParams.morphWindowName, WINDOW_AUTOSIZE);
	moveWindow(this->configParams.morphWindowName, int(this->configParams.screenWidth / 3), 0);
	resizeWindow(this->configParams.morphWindowName, int(configParams.newCols), int(configParams.newRows));

	createTrackbar("MorphType", this->configParams.morphWindowName, &this->configParams.morphTransformType, 8, morphCallback, &this->configParams);
	createTrackbar("KernelSize", this->configParams.morphWindowName, &this->configParams.kernel_morph_size, 5, morphCallback, &this->configParams);
	//createTrackbar("MorphShape", this->configParams.morphWindowName, &this->configParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams);
	
	morphParametersP2();
	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
}
void CalibParams::morphCallback(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
	int operation = configP->morphTransformType + 1;

	Mat morphImg = configP->cannyImg.clone();

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

void CalibParams::edgeParametersP() {

	namedWindow(this->configParams.edgeWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeWindowName, int(configParams.newCols), int(configParams.newRows));
	moveWindow(this->configParams.edgeWindowName, 0, int(this->configParams.screenHeight / 2));

	createTrackbar("BlurLevel", this->configParams.edgeWindowName, &this->configParams.gauss_ksize, 15, edgeDetectCallback, &this->configParams);
	createTrackbar("CannyLow", this->configParams.edgeWindowName, &this->configParams.lowThresh, 100, edgeDetectCallback, &this->configParams);
	createTrackbar("CannyHigh", this->configParams.edgeWindowName, &this->configParams.highThresh, 150, edgeDetectCallback, &this->configParams);

	morphParametersP();
	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
};
void CalibParams::edgeDetectCallback(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
	int operation = configP->morphTransformType + 1;
	int cannyKernelSize = configP->cannyKernel + 3;
	cv::Mat cannyImg;
	Mat img_ = configP->roiImg.clone();

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

void CalibParams::blurThreshParametersP() {

	namedWindow(this->configParams.blurThreshWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.blurThreshWindowName, int(configParams.newCols), int(configParams.newRows));
	moveWindow(this->configParams.blurThreshWindowName, 0, 0);

	createTrackbar("BlurLevel", this->configParams.blurThreshWindowName, &this->configParams.gauss_ksize, 10, blurThreshCallback, &this->configParams);
	//createTrackbar("TheshLevel", this->configParams.BlurThreshWindowName, &this->configParams.thresBin, 255, blurThreshCallback, &this->configParams);

	edgeParametersP();
	waitKey();
	if (!this->configParams.terminateSetup) {
		destroyAllWindows();
		updateParams();
		this->configParams.terminateSetup = true;
	}
}
void CalibParams::blurThreshCallback(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);
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

void CalibParams::setup(CalibParams & cb_, cv::Mat & img)
{
	int maxHeight = int(cb_.configParams.screenHeight / 2);
	int maxWidth = int(cb_.configParams.screenWidth / 3);

	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;
	}

	else {
		new_rows = maxHeight;
	}

	cb_.configParams.newCols = new_cols;
	cb_.configParams.newRows = new_rows;
	cb_.configParams.recHeight = new_rows;
	cb_.configParams.recWidth = new_cols;

	cv::Mat m;
	const std::string winName = "Background";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	m.create(int(cb_.configParams.screenHeight*0.95), cb_.configParams.screenWidth, CV_32FC3);
	m.setTo(cv::Scalar(255, 255, 255));
	moveWindow(winName, 0, 0);
	cv::imshow(winName, m);

	cb_.configParams.origImg = img.clone();
	cb_.setLane_ROIBox();
	cb_.setCarDetectionROIBox();

}

void CalibParams::setLane_ROIBox() {
	namedWindow(this->configParams.roiBoxWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.roiBoxWindowName, int(configParams.newCols), int(configParams.newRows));
	moveWindow(this->configParams.roiBoxWindowName, 0, 0);
	size_t max_val_x = this->configParams.origImg.cols;
	size_t max_val_y = this->configParams.origImg.rows;

	createTrackbar("X1-Pos", this->configParams.roiBoxWindowName, &this->configParams.x1_roi, max_val_x, roiLaneCallback, &this->configParams);
	createTrackbar("RecWidth", this->configParams.roiBoxWindowName, &this->configParams.recWidth, max_val_x, roiLaneCallback, &this->configParams);
	createTrackbar("Y1-Pos", this->configParams.roiBoxWindowName, &this->configParams.y1_roi, max_val_y, roiLaneCallback, &this->configParams);
	createTrackbar("RecHeight", this->configParams.roiBoxWindowName, &this->configParams.recHeight, max_val_y, roiLaneCallback, &this->configParams);

	edgeParametersP();
	waitKey();
	//destroyAllWindows();
	//destroyWindow(this->configParams.blurThreshWindowName);
}
void CalibParams::roiLaneCallback(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);  // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);


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
	namedWindow(this->configParams.roiBoxWindowName_car, WINDOW_AUTOSIZE);
	//moveWindow(this->configParams.roiBoxWindowName_car, 0, 0);
	size_t max_val_x = this->configParams.origImg.cols;
	size_t max_val_y = this->configParams.origImg.rows;

	createTrackbar("X1-Pos", this->configParams.roiBoxWindowName_car, &this->configParams.x1_roi_car, max_val_x, roiCarCallback, &this->configParams);
	createTrackbar("RecWidth", this->configParams.roiBoxWindowName_car, &this->configParams.recWidth_car, max_val_x, roiCarCallback, &this->configParams);
	createTrackbar("Y1-Pos", this->configParams.roiBoxWindowName_car, &this->configParams.y1_roi_car, max_val_y, roiCarCallback, &this->configParams);
	createTrackbar("RecHeight", this->configParams.roiBoxWindowName_car, &this->configParams.recHeight_car, max_val_y, roiCarCallback, &this->configParams);
	waitKey();
	destroyAllWindows();
};

void CalibParams::roiCarCallback(int, void *userdata) {
	ConfigParams *configP = reinterpret_cast<ConfigParams *>(userdata);   // cvtColor(params->colImg(params->config->roiBbox), params->grayImg, CV_RGB2GRAY);
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
void CalibParams::displayImg(Mat img, const std::string title, int screenWidth, int screenHeight, int img_num) {

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
void CalibParams::drawLines(ConfigParams* configParams, cv::Mat& img, std::vector<cv::Vec4i> lines, bool laneDetection) {

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
	houghVar.minVotes = configParams.minVotes;
	houghVar.minLineLength = configParams.minLineLength;
	houghVar.maxLineGap = configParams.maxLineGap;
	houghVar.lineThickness = configParams.lineThickness;

	preprocessVar.cannyKernelSize = configParams.cannyKernel;
	preprocessVar.cannyHighTresh = configParams.highThresh;
	preprocessVar.cannyLowThresh = configParams.lowThresh;
	preprocessVar.gaussKernelSize = configParams.gauss_ksize;
	preprocessVar.morphElemShape = configParams.morph_elem_shape;
	preprocessVar.morphTransformType1 = configParams.morphTransformType;
	preprocessVar.morphKernelSize1 = configParams.kernel_morph_size;
	preprocessVar.morphTransformType2 = configParams.morphTransformType_;
	preprocessVar.morphKernelSize2 = configParams.kernel_morph_size_;

}

LineDetector::preprocessParams CalibParams::getPreprocessParams()
{
	return preprocessVar;
}

LineDetector::houghParams CalibParams::getHoughParams()
{
	return houghVar;
}











