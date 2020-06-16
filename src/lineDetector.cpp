#include <lineDetector.h>
using namespace cv;
using namespace std;


LineDetector::LineDetector() {
	struct ConfigParams configParams;
}

LineDetector:: ~LineDetector() {}; 

// int(this->configParams.edgeParams.screenLength / 2), int(this->configParams.edgeParams.screenWidth / 2));



void LineDetector::houghParametersP() {
	//static LDParam params;
	//create window for trackbars
	namedWindow(this->configParams.edgeParams.houghWindowName, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->configParams.edgeParams.houghWindowName, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.houghWindowName, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));

	//create memory to store trackbar name on window
	createTrackbar("minVotes", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minVotes, this->configParams.edgeParams.minVotesLim, houghPCallback, &this->configParams.edgeParams);
	createTrackbar("minLength", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.minLineLength, this->configParams.edgeParams.minLineLengthLimit, houghPCallback, &this->configParams.edgeParams);
	createTrackbar("maxGap", this->configParams.edgeParams.houghWindowName, &this->configParams.edgeParams.maxLineGap, this->configParams.edgeParams.maxLineGapLimit, houghPCallback, &this->configParams.edgeParams);
	//createTrackbar("cannyLow", params.windowName, &params.lowCannyThreshold, 255, LineDetector::houghPCallback, &params);
	//createTrackbar("cannyHigh", params.windowName, &params.highCannyThreshold, 255, LineDetector::houghPCallback, &params);
	//createTrackbar("kernelSize", params.windowName, &params.apertureSize, 21, LineDetector::houghPCallback, &params);
	waitKey();
	destroyWindow(this->configParams.edgeParams.houghWindowName);
};



void LineDetector::houghPCallback(int, void *userdata) {
	
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	Mat houghImg = edgeConfig->morphImg2.clone();
	vector<Vec4i> lines;
	HoughLinesP(houghImg, lines, 1, CV_PI / 180, edgeConfig->minVotes, edgeConfig->minLineLength, edgeConfig->maxLineGap);

	Mat houghTransform = edgeConfig->origImg.clone();
	
	edgeConfig -> edgeLines = lines;
	cout << "\n lines.size() = " << lines.size();

	for (auto it = lines.begin(); it != lines.end(); ++it) {
		Point a, b;
		auto line = *it;
		int x = line[0] + line[1] + line[2] + line[3];

		if (x != 0) {
			a.x = line[0];
			a.y = line[1];
			b.x = line[2];
			b.y = line[3];
			cv::line(houghTransform, a, b, Scalar(0, 0, 255), edgeConfig->lineThickness, LINE_AA);
		}
	}

	int img_num = 3;
	displayImg(houghTransform, edgeConfig->houghWindowName, edgeConfig->screenWidth, edgeConfig->screenHeight, img_num);
	// cv::imshow(params->windowName, temp);

}

void LineDetector::morphParametersP2() {

	namedWindow(this->configParams.edgeParams.morphWindowName2, WINDOW_AUTOSIZE);

	//imshow(title, Img); // Show our image inside it.  // 
	moveWindow(this->configParams.edgeParams.morphWindowName2, 0, int(this->configParams.edgeParams.screenHeight / 2));
	resizeWindow(this->configParams.edgeParams.morphWindowName2, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.morphTransformType_, 8, morphCallback2, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName2, &this->configParams.edgeParams.kernel_morph_size_, 21, morphCallback2, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);
	
	houghParametersP();
	waitKey();
	destroyWindow(this->configParams.edgeParams.morphWindowName2);
}

void LineDetector::morphCallback2(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType_ + 1;

	Mat morphImg2 = edgeConfig->morphImg.clone();
	cout << "operation = " << operation << endl;
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
	houghPCallback(0, edgeConfig);
}


void LineDetector::morphParametersP() {  
	
	namedWindow(this->configParams.edgeParams.morphWindowName, WINDOW_AUTOSIZE);
	
	//imshow(title, Img); // Show our image inside it.
	moveWindow(this->configParams.edgeParams.morphWindowName, int(this->configParams.edgeParams.screenWidth / 2), 0);
	resizeWindow(this->configParams.edgeParams.morphWindowName, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));

	createTrackbar("MorphType", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morphTransformType, 8, morphCallback, &this->configParams.edgeParams);
	createTrackbar("KernelSize", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.kernel_morph_size, 21, morphCallback, &this->configParams.edgeParams);
	//createTrackbar("MorphShape", this->configParams.edgeParams.morphWindowName, &this->configParams.edgeParams.morph_elem_shape, 2, edgeDetectCallback, &this->configParams.edgeParams);
	morphParametersP2();
	waitKey();
	destroyWindow(this->configParams.edgeParams.morphWindowName);
}

void LineDetector::morphCallback(int, void *userdata) {
	EdgeConfig *edgeConfig = reinterpret_cast<EdgeConfig *>(userdata);
	int operation = edgeConfig->morphTransformType + 1;

	Mat morphImg = edgeConfig->cannyImg.clone();
	cout << "operation = " << operation << endl;  
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
	resizeWindow(this->configParams.edgeParams.edgeWindowName, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));
	moveWindow(this->configParams.edgeParams.edgeWindowName, 0, 0);
	
	createTrackbar("BlurLevel", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.gauss_ksize, 15, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyLow", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.lowThresh, 500, edgeDetectCallback, &this->configParams.edgeParams);
	createTrackbar("CannyHigh", this->configParams.edgeParams.edgeWindowName, &this->configParams.edgeParams.highThresh, 455, edgeDetectCallback, &this->configParams.edgeParams);

	morphParametersP();
	waitKey();
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

};

void LineDetector::blurThreshParametersP() {

	namedWindow(this->configParams.edgeParams.blurThreshWindowName, WINDOW_AUTOSIZE);
	resizeWindow(this->configParams.edgeParams.blurThreshWindowName, int(this->configParams.edgeParams.screenWidth / 2), int(this->configParams.edgeParams.screenHeight / 2));
	moveWindow(this->configParams.edgeParams.blurThreshWindowName, 0, 0);

	createTrackbar("BlurLevel", this->configParams.edgeParams.blurThreshWindowName, &this->configParams.edgeParams.gauss_ksize, 10, blurThreshCallback, &this->configParams.edgeParams);
	//createTrackbar("TheshLevel", this->configParams.edgeParams.BlurThreshWindowName, &this->configParams.edgeParams.thresBin, 255, blurThreshCallback, &this->configParams.edgeParams);

	edgeParametersP();
	waitKey();
	destroyWindow(this->configParams.edgeParams.blurThreshWindowName);
};


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
};


void LineDetector::displayImg(Mat img, const std::string title, int screenWidth, int screenHeight, int img_num) {

	double img_width = img.cols;  //obtain size of image and halve it in order to fit 
	double img_height = img.rows;
	
	if (img_num != -1) {
		int maxRows = int(screenHeight / 2);
		int maxCols = int(screenWidth / 2);
		Mat img_;

		if (img.cols < img.rows) {
			resize(img, img_, cv::Size(), maxRows / img_height, maxRows / img_height);
		}

		else {
			resize(img, img_, cv::Size(), maxCols / img_width, maxCols / img_width);
		}

		if (img_num == 0) {
			moveWindow(title, 0, 0);
		}
		else if (img_num == 1) {
			moveWindow(title, maxCols, 0);
		}
		else if (img_num == 2) {
			moveWindow(title, 0, maxRows);
		}

		else if (img_num == 3) {
			moveWindow(title, maxCols, maxRows);
		}

		imshow(title, img_);
	}

	imshow(title, img);

};

















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

