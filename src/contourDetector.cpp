#include <contourDetector.h>
using namespace cv;
using namespace std;



void ContourDetector::detectObject() {

	vector<vector<Point>> contours_;
	vector<Vec4i> hierarchy;

	cv::Mat temp;
	preprocessImg.roiImg = getCurrImg()(roi_Bbox).clone();
	if (preprocessVar.gaussKernelSize > 0) {
		cvtColor(preprocessImg.roiImg, temp, COLOR_BGR2GRAY);
		blur(temp, temp, Size(preprocessVar.gaussKernelSize, preprocessVar.gaussKernelSize), Point(-1, -1));
	}

	// 3. Canny Edge Detection
	if (!temp.empty()) {
		Canny(temp, temp, preprocessVar.cannyLowThresh, preprocessVar.cannyHighTresh, preprocessVar.cannyKernelSize + 3);
		//setCannyImg(temp.clone());
	}
	findContours(temp, contours_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (int k = 0; k < contours_.size(); k++) {
		approxPolyDP(contours_[k], contours_[k], epsilon, true);
	}
	contours = contours_;


}