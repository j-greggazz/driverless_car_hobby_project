#include <lineDetector.h>

using namespace cv;
using namespace std;


LineDetector::LineDetector(int iD)
{
	setId(iD);
}


LineDetector::LineDetector()
{
}

LineDetector::~LineDetector()
{
}



void LineDetector::preprocImg()
{
	cv::Mat temp;
	// 1. Define new ROI img
	//setRoiImg(getCurrImg()(getRoiBox()));
	preprocessImg.roiImg = getCurrImg()(getRoiBox()).clone();

	// 2. Blur
	if (getGaussKernelSize() > 0) {
		cvtColor(getRoiImg(), temp, COLOR_BGR2GRAY);
		blur(temp, temp, Size(getGaussKernelSize(), getGaussKernelSize()), Point(-1, -1));
	}

	// 3. Canny Edge Detection
	if (!temp.empty()) {
		Canny(temp, temp, getCannyLowThresh(), getCannyHighThresh(), getCannyKernelSize());
		setCannyImg(temp.clone());
	}

	// 4. Morphological Operations 1
	if (getMorphTransformType1() + 1 > 3) {
		Mat str_element = getStructuringElement(getMorphElemShape(), Size(2 * getMorphKernelSize1() + 1, 2 * getMorphKernelSize1() + 1), Point(getMorphKernelSize1(), getMorphKernelSize1()));
		morphologyEx(temp, temp, getMorphTransformType1() - 1, str_element);
	}

	else if (getMorphTransformType1() + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(getMorphKernelSize1() + 1, getMorphKernelSize1() + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (getMorphTransformType1() + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(getMorphKernelSize1() + 1, getMorphKernelSize1() + 1), Point(getMorphKernelSize1(), getMorphKernelSize1()));
		dilate(temp, temp, element);
	}

	// 5. Morphological Operations 2
	if (getMorphTransformType2() + 1 > 3) {
		Mat element = getStructuringElement(getMorphElemShape(), Size(2 * getMorphKernelSize2() + 1, 2 * getMorphKernelSize2() + 1), Point(getMorphKernelSize2(), getMorphKernelSize2()));
		morphologyEx(temp, temp, getMorphTransformType1() - 1, element);
	}

	else if (getMorphTransformType1() + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(getMorphKernelSize2() + 1, getMorphKernelSize2() + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (getMorphTransformType1() + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(getMorphKernelSize2() + 1, getMorphKernelSize2() + 1), Point(getMorphKernelSize1(), getMorphKernelSize2()));
		dilate(temp, temp, element);
	}

	// 6. Set Input Image to Hough Transform
	setHoughImg(temp);
}

void LineDetector::detectLines()
{
	/*try {
		std::vector<cv::Vec4i> linesTemp;
		cv::HoughLinesP(getHoughImg(), linesTemp, 1, CV_PI / 180, houghVar.minVotes, houghVar.minLineLength, houghVar.maxLineGap);

	}
	catch (cv::Exception& e) {
		//ignore here for now
	}*/
	std::vector<cv::Vec4i> linesTemp;
	cv::HoughLinesP(getHoughImg(), linesTemp, 1, CV_PI / 180, houghVar.minVotes, houghVar.minLineLength, houghVar.maxLineGap);
	houghVar.lines = linesTemp;

}

void LineDetector::detectObject()
{
	preprocImg();
	detectLines();
}

void LineDetector::drawLines(LineDetector & ld, bool detectLanes)
{
	int x_offset = ld.get_x1_roi();
	int y_offset = ld.get_y1_roi();
	float lane_1_m = 0.;
	float lane_2_m = 0.;
	int count_1 = 0;
	float lane_1_yc = 0.;
	float lane_2_yc = 0.;
	int count_2 = 0;
	Point roi_lane1_pt1, roi_lane1_pt2, roi_lane2_pt1, roi_lane2_pt2;

	for (auto it = ld.houghVar.lines.begin(); it != ld.houghVar.lines.end(); ++it) {
		Point a, b;
		auto line = *it;
		int x = line[0] + line[1] + line[2] + line[3];

		if (x != 0) {
			a.x = line[0] + x_offset;
			a.y = line[1] + y_offset;
			b.x = line[2] + x_offset;
			b.y = line[3] + y_offset;
			if (detectLanes) {
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
				cv::line(ld.getCurrImg(), a, b, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
			}
		}
	}
	if (detectLanes) {
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
		dashboardLane1.y = ld.getCurrImg().rows;
		dashboardLane2.y = ld.getCurrImg().rows;

		if (roi_lane1_pt1.y < roi_lane1_pt2.y) {
			line1End = roi_lane1_pt1;
			dashboardLane1.x = (ld.getCurrImg().rows - roi_lane1_pt1.y) / avgMLane1 + roi_lane1_pt1.x;
		}

		else {
			line1End = roi_lane1_pt2;
			dashboardLane1.x = (ld.getCurrImg().rows - roi_lane1_pt2.y) / avgMLane1 + roi_lane1_pt2.x;
		}

		if (roi_lane2_pt1.y < roi_lane2_pt2.y) {
			line2End = roi_lane2_pt1;
			dashboardLane2.x = (ld.getCurrImg().rows - roi_lane2_pt1.y) / avgMLane2 + roi_lane2_pt1.x;
		}

		else {
			line2End = roi_lane2_pt2;
			dashboardLane2.x = (ld.getCurrImg().rows - roi_lane2_pt2.y) / avgMLane2 + roi_lane2_pt2.x;
		}

		if (count_1 > 0) {
			cv::arrowedLine(ld.getCurrImg(), dashboardLane1, line1End, Scalar(0, 255, 0), ld.houghVar.lineThickness, LINE_AA);
			ld.houghVar.line1_pt1 = line1End;
			ld.houghVar.line1_pt2 = dashboardLane1;
		}
		else {
			cv::arrowedLine(ld.getCurrImg(), ld.houghVar.line1_pt1, ld.houghVar.line1_pt2, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
		}

		if (count_2 > 0) {
			cv::arrowedLine(ld.getCurrImg(), dashboardLane2, line2End, Scalar(0, 255, 0), ld.houghVar.lineThickness, LINE_AA);
			ld.houghVar.line2_pt1 = line2End;
			ld.houghVar.line2_pt2 = dashboardLane2;
		}
		else {
			cv::arrowedLine(ld.getCurrImg(), ld.houghVar.line2_pt1, ld.houghVar.line2_pt2, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
		}
	}

}


// Implementations Getters & Setters

void LineDetector::setCannyKernelSize(int canny_KernelSize)
{
	preprocessVar.cannyKernelSize = canny_KernelSize;
}

void LineDetector::setCannyLowThresh(int canny_LowThresh)
{
	preprocessVar.cannyLowThresh = canny_LowThresh;
}

void LineDetector::setCannyHighThresh(int canny_HighThresh)
{
	preprocessVar.cannyHighTresh = canny_HighThresh;
}

void LineDetector::setGaussKernelSize(int gauss_KernelSize)
{
	preprocessVar.gaussKernelSize = gauss_KernelSize;
}

void LineDetector::setMorphElemShape(int morph_ElemShape)
{
	preprocessVar.morphElemShape = morph_ElemShape;
}

void LineDetector::setMorphTransformType1(int morph_TransformType1)
{
	preprocessVar.morphTransformType1 = morph_TransformType1;
}

void LineDetector::setMorphTransformType2(int morph_TransformType2)
{
	preprocessVar.morphTransformType2 = morph_TransformType2;
}




void LineDetector::setMorphKernelSize1(int morph_KernelSize1)
{
	preprocessVar.morphKernelSize1 = morph_KernelSize1;
}

void LineDetector::setMorphKernelSize2(int morph_KernelSize2)
{
	preprocessVar.morphKernelSize2 = morph_KernelSize2;
}

LineDetector::preprocessParams LineDetector::getPreprocessParams()
{
	return preprocessVar;
}

LineDetector::houghParams LineDetector::getHoughParams()
{
	return houghVar;
}

void LineDetector::setRoiImg(cv::Mat & roi_Img)
{
	preprocessImg.roiImg = roi_Img;
}

void LineDetector::setBlurImg(cv::Mat & blur_Img)
{
	preprocessImg.blurImg = blur_Img;
}

void LineDetector::setGrayImg(cv::Mat & gray_Img)
{
	preprocessImg.grayImg = gray_Img;
}

void LineDetector::setCannyImg(cv::Mat & canny_Img)
{
	preprocessImg.cannyImg = canny_Img;
}

void LineDetector::setMorphImg(cv::Mat & morph_Img)
{
	preprocessImg.morphImg = morph_Img;
}

void LineDetector::setHoughImg(cv::Mat & hough_Img)
{
	preprocessImg.houghImg = hough_Img;
}

int LineDetector::getCannyKernelSize()
{
	return preprocessVar.cannyKernelSize;
}

int LineDetector::getCannyLowThresh()
{
	return preprocessVar.cannyLowThresh;
}

int LineDetector::getCannyHighThresh()
{
	return preprocessVar.cannyHighTresh;
}

int LineDetector::getGaussKernelSize()
{
	return preprocessVar.gaussKernelSize;
}

int LineDetector::getMorphElemShape()
{
	return preprocessVar.morphElemShape;
}

int LineDetector::getMorphTransformType1()
{
	return preprocessVar.morphTransformType1;
}

int LineDetector::getMorphTransformType2()
{
	return preprocessVar.morphTransformType2;
}


int LineDetector::getMorphKernelSize1()
{
	return preprocessVar.morphKernelSize1;
}

int LineDetector::getMorphKernelSize2()
{
	return preprocessVar.morphKernelSize2;
}

cv::Mat LineDetector::getRoiImg()
{
	return preprocessImg.roiImg;
}

cv::Mat LineDetector::getBlurImg()
{
	return preprocessImg.blurImg;
}

cv::Mat LineDetector::getGrayImg()
{
	return preprocessImg.grayImg;
}

cv::Mat LineDetector::getCannyImg()
{
	return preprocessImg.cannyImg;
}

cv::Mat LineDetector::getMorphImg()
{
	return preprocessImg.morphImg;
}

cv::Mat LineDetector::getHoughImg()
{
	return preprocessImg.houghImg;
}

// Parameter initialisation

 void LineDetector::setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox)
{
	 houghVar = hParams;
	 preprocessVar = pParams;
	 roi_Bbox = roi_Bbox;
	 /*
	 lD.set_x1_roi();
	 lD.set_y1_roi();
	 lD.setRecWidth();
	 lD.setRecHeight();
	 lD.setRoiBox();
	 lD.setCannyKernelSize();
	 lD.setCannyLowThresh();
	 lD.setCannyHighThresh();
	 lD.setGaussKernelSize();
	 lD.setMorphElemShape();
	 lD.setMorphTransformType1();
	 lD.setMorphTransformType2();
	 lD.setMorphOperation1();
	 lD.setMorphOperation2();
	 lD.setMorphKernelSize1();
	 lD.setMorphKernelSize2();
	 */
}
