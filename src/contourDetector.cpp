#include <contourDetector.h>
using namespace cv;
using namespace std;

// Not appropriate for task - method not implemented 

void ContourDetector::detectObject() {

	vector<vector<Point>> contours_;

	vector<Vec4i> hierarchy;
	setImgProcessed(false);
	cv::Mat temp, temp2;
	m_preprocessImg.roiImg = LineDetector::getCurrImg()(getRoiBox()).clone();
	
	// 1. Blur
	if (m_preprocessVar.gaussKernelSize > 0) {
		cvtColor(m_preprocessImg.roiImg, temp, COLOR_BGR2GRAY);
		blur(temp, temp2, Size(m_preprocessVar.gaussKernelSize, m_preprocessVar.gaussKernelSize), Point(-1, -1));
		GaussianBlur(temp2, temp, Size(5, 5), 2, 2);
	}

	// 2. Canny Edge Detection
	if (!temp.empty()) {
		Canny(temp, temp, m_preprocessVar.cannyLowThresh, m_preprocessVar.cannyHighTresh, m_preprocessVar.cannyKernelSize + 3);
		//setCannyImg(temp.clone());
	}

	// 3. Remove Noise
	if (!temp.empty()) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(m_preprocessVar.morphKernelSize1 + 1, m_preprocessVar.morphKernelSize1 + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		dilate(temp, temp, element, Point(-1, -1), 1, 1, 1);
		erode(temp, temp, element, Point(-1, -1), 1, 1, 1);
		dilate(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}
	std::vector<cv::Vec4i> linesTemp;
	cv::HoughLinesP(temp, linesTemp, 1, CV_PI / 180, m_houghVar.minVotes, m_houghVar.minLineLength, m_houghVar.maxLineGap);
	m_houghVar.lines = linesTemp;
	int x_offset = getRoiBox().x;
	int y_offset = getRoiBox().y;
	m_houghVar.lines.clear();
	
	temp2 = getCurrImg();
	for (auto it = linesTemp.begin(); it != linesTemp.end(); ++it) {
		Point a, b;
		auto line = *it;
		int x = line[0] + line[1] + line[2] + line[3];

		if (x != 0) {
			a.x = line[0] + x_offset;
			a.y = line[1] + y_offset;
			b.x = line[2] + x_offset;
			b.y = line[3] + y_offset;

			float m = float(b.y - a.y) / float(b.x - a.x);
			float yc = b.y - m * a.y;
			if (((m >= 0.43 & m <= 0.58) & (abs(a.y - b.y) > 10)) | (m >= -0.75  & m <= -0.62)) {
				//houghVar.lines.push_back(line);
				cv::line(temp2, a, b, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
			}
		}
	}

	


	/*
	findContours(temp, contours_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> contours_approx = contours_;
	//contours.clear();
	int thresh = 10;
	for (int k = 0; k < contours_.size(); k++) {
		approxPolyDP(contours_[k], contours_approx[k], epsilon, true);
		if (contours_approx[k].size() == 4 & cv::contourArea(contours_approx[k]) > thresh) {
			//contours.push_back(contours_approx[k]);
			drawContours(preprocessImg.roiImg, contours_approx, k, (0, 0, 255), cv::FILLED, 8, hierarchy);
		}
	}

	double maxArea = 0;
	double area;
	int iterK;*/
	/*
	for (int k = 0; k < contours_.size(); k++) {
		area = cv::contourArea(contours_[k]);
		if (area > maxArea) {
			maxArea = area;
			iterK = k;
		}
	}*/

	//contours = contours_;


	m_showImg = m_preprocessImg.roiImg;
	m_showImg = temp2;
	setImgProcessed(true);

}

cv::Mat ContourDetector::getShowImg() const
{
	return m_showImg;
}

void ContourDetector::setParams(const preprocessParams& pParams, const houghParams& hParams, const cv::Rect& roi_Bbox)
{
	m_houghVar = hParams;
	m_preprocessVar = pParams;
	ObjectDetector::setRoiBox(roi_Bbox);
}


