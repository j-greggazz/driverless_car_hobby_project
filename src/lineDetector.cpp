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
	preprocessImg.roiImg = getCurrImg()(getRoiBox()).clone();

	// 2. Blur
	if (preprocessVar.gaussKernelSize > 0) {
		cvtColor(preprocessImg.roiImg, temp, COLOR_BGR2GRAY);
		blur(temp, temp, Size(preprocessVar.gaussKernelSize, preprocessVar.gaussKernelSize), Point(-1, -1));
	}

	// 3. Canny Edge Detection
	if (!temp.empty()) {
		Canny(temp, temp, preprocessVar.cannyLowThresh, preprocessVar.cannyHighTresh, preprocessVar.cannyKernelSize + 3);
		//setCannyImg(temp.clone());
	}

	// 4. Morphological Operations 1
	if (preprocessVar.morphTransformType1 + 1 > 3) {
		Mat str_element = getStructuringElement(preprocessVar.morphElemShape, Size(2 * preprocessVar.morphKernelSize1 + 1, 2 * preprocessVar.morphKernelSize1 + 1), Point(preprocessVar.morphKernelSize1, preprocessVar.morphKernelSize1));
		morphologyEx(temp, temp, preprocessVar.morphTransformType1 - 1, str_element);
	}

	else if (preprocessVar.morphTransformType1 + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(preprocessVar.morphKernelSize1 + 1, preprocessVar.morphKernelSize1 + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (preprocessVar.morphTransformType1 + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(preprocessVar.morphKernelSize1 + 1, preprocessVar.morphKernelSize1 + 1), Point(preprocessVar.morphKernelSize1, preprocessVar.morphKernelSize1));
		dilate(temp, temp, element);
	}

	// 5. Morphological Operations 2
	if (preprocessVar.morphTransformType2 + 1 > 3) {
		Mat element = getStructuringElement(preprocessVar.morphElemShape, Size(2 * preprocessVar.morphKernelSize2 + 1, 2 * preprocessVar.morphKernelSize2 + 1), Point(preprocessVar.morphKernelSize2, preprocessVar.morphKernelSize2));
		morphologyEx(temp, temp, preprocessVar.morphTransformType2 - 1, element);
	}

	else if (preprocessVar.morphTransformType2 + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(preprocessVar.morphKernelSize2 + 1, preprocessVar.morphKernelSize2 + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (preprocessVar.morphTransformType2 + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(preprocessVar.morphKernelSize2 + 1, preprocessVar.morphKernelSize2 + 1), Point(preprocessVar.morphKernelSize2, preprocessVar.morphKernelSize2));
		dilate(temp, temp, element);
	}

	// 6. Set Input Image to Hough Transform
	preprocessImg.houghImg = temp;
	//setHoughImg(temp);
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
	cv::HoughLinesP(preprocessImg.houghImg, linesTemp, 1, CV_PI / 180, houghVar.minVotes, houghVar.minLineLength, houghVar.maxLineGap);
	houghVar.lines = linesTemp;

}

void LineDetector::setLines(std::vector<cv::Vec4i> lines_)
{
	houghVar.lines = lines_;
}

void LineDetector::detectObject()
{
	preprocImg();
	detectLines();
}

void LineDetector::drawLines(LineDetector & ld, Mat& img, bool detectLanes)
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
				cv::line(img, a, b, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
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
			cv::arrowedLine(img, dashboardLane1, line1End, Scalar(0, 255, 0), ld.houghVar.lineThickness, LINE_AA);
			ld.houghVar.line1_pt1 = line1End;
			ld.houghVar.line1_pt2 = dashboardLane1;
		}
		else {
			cv::arrowedLine(img, ld.houghVar.line1_pt1, ld.houghVar.line1_pt2, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
		}

		if (count_2 > 0) {
			cv::arrowedLine(img, dashboardLane2, line2End, Scalar(0, 255, 0), ld.houghVar.lineThickness, LINE_AA);
			ld.houghVar.line2_pt1 = line2End;
			ld.houghVar.line2_pt2 = dashboardLane2;
		}
		else {
			cv::arrowedLine(img, ld.houghVar.line2_pt1, ld.houghVar.line2_pt2, Scalar(0, 0, 255), ld.houghVar.lineThickness, LINE_AA);
		}
	}

}


// Implementations Getters & Setters

LineDetector::preprocessParams LineDetector::getPreprocessParams()
{
	return preprocessVar;
}

LineDetector::houghParams LineDetector::getHoughParams()
{
	return houghVar;
}






// Parameter initialisation

 void LineDetector::setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox)
{
	 houghVar = hParams;
	 preprocessVar = pParams;
	 ObjectDetector::setRoiBox(roi_Bbox);

}
