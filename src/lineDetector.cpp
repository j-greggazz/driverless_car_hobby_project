#include <lineDetector.h>

using namespace cv;
using namespace std;



void LineDetector::preprocImg()
{
	cv::Mat temp;
	m_preprocessImg.roiImg = getCurrImg()(getRoiBox()).clone();

#if HAS_CUDA
	setImgProcessed(false);
	cv::cuda::GpuMat gpuImg, grayImg, mask;
	gpuImg.upload(m_preprocessImg.roiImg);
	cv::cuda::cvtColor(gpuImg, grayImg, COLOR_BGR2GRAY);

	// 2. Blur
	if (m_preprocessVar.gaussKernelSize > 0) {
		m_BoxFilter->apply(grayImg, grayImg);
	}

	//grayImg.download(temp);
	//imshow("Blurred", temp);
	//waitKey();
	// 3. Canny Edge Detection
	if (!grayImg.empty()) {
		m_CannyDetector->detect(grayImg, mask);
	}

	//mask.download(temp);
	//imshow("Canny Image", temp);
	//waitKey();
	// 4. Morphological Operations
	//m_morphFilter_1->apply(mask, mask);
	//mask.download(temp);
	//imshow("Morph1", temp);
	//waitKey();
	//m_morphFilter_2->apply(mask, mask);
	//mask.download(temp);
	//imshow("Morph2", temp);
	//waitKey();

	// 5. Detect Lines
    //std::vector<cv::Vec4i> d_lines;
	//m_HoughDetector->detect(mask, d_lines);
	//m_houghVar.lines = d_lines;

	cv::cuda::GpuMat d_lines2;
	m_HoughDetector->detect(mask, d_lines2);

	if (!d_lines2.empty()) {
		m_lines.resize(d_lines2.cols);
		Mat h_lines(1, d_lines2.cols, CV_32SC4, &m_lines[0]);
		d_lines2.download(h_lines);
	}

	m_houghVar.lines = m_lines;
	//temp = getCurrImg().clone();
	//drawLines(temp);
	//imshow("Hough lines detected", temp);
	//finishedImg = temp;
	//waitKey(10);
	//setImgProcessed(true);

#else

	
	// 2. Blur
	if (m_preprocessVar.gaussKernelSize > 0) {
		cvtColor(m_preprocessImg.roiImg, temp, COLOR_BGR2GRAY);
		blur(temp, temp, Size(m_preprocessVar.gaussKernelSize, m_preprocessVar.gaussKernelSize), Point(-1, -1));
	}

	// 3. Canny Edge Detection
	if (!temp.empty()) {
		Canny(temp, temp, m_preprocessVar.cannyLowThresh, m_preprocessVar.cannyHighTresh, m_preprocessVar.cannyKernelSize + 3);
		//setCannyImg(temp.clone());
	}

	// 4. Morphological Operations 1
	if (m_preprocessVar.morphTransformType1 + 1 > 3) {
		Mat str_element = getStructuringElement(m_preprocessVar.morphElemShape, Size(2 * m_preprocessVar.morphKernelSize1 + 1, 2 * m_preprocessVar.morphKernelSize1 + 1), Point(m_preprocessVar.morphKernelSize1, m_preprocessVar.morphKernelSize1));
		morphologyEx(temp, temp, m_preprocessVar.morphTransformType1 - 1, str_element);
	}

	else if (m_preprocessVar.morphTransformType1 + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(m_preprocessVar.morphKernelSize1 + 1, m_preprocessVar.morphKernelSize1 + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (m_preprocessVar.morphTransformType1 + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(m_preprocessVar.morphKernelSize1 + 1, m_preprocessVar.morphKernelSize1 + 1), Point(m_preprocessVar.morphKernelSize1, m_preprocessVar.morphKernelSize1));
		dilate(temp, temp, element);
	}

	// 5. Morphological Operations 2
	if (m_preprocessVar.morphTransformType2 + 1 > 3) {
		Mat element = getStructuringElement(m_preprocessVar.morphElemShape, Size(2 * m_preprocessVar.morphKernelSize2 + 1, 2 * m_preprocessVar.morphKernelSize2 + 1), Point(m_preprocessVar.morphKernelSize2, m_preprocessVar.morphKernelSize2));
		morphologyEx(temp, temp, m_preprocessVar.morphTransformType2 - 1, element);
	}

	else if (m_preprocessVar.morphTransformType2 + 1 == 1) {
		Mat element = getStructuringElement(MORPH_CROSS, Size(m_preprocessVar.morphKernelSize2 + 1, m_preprocessVar.morphKernelSize2 + 1));//, Point(edgeConfig->kernel_morph_size, edgeConfig->kernel_morph_size));
		erode(temp, temp, element, Point(-1, -1), 2, 1, 1);
	}

	else if (m_preprocessVar.morphTransformType2 + 1 == 2) {
		Mat element = getStructuringElement(MORPH_RECT, Size(m_preprocessVar.morphKernelSize2 + 1, m_preprocessVar.morphKernelSize2 + 1), Point(m_preprocessVar.morphKernelSize2, m_preprocessVar.morphKernelSize2));
		dilate(temp, temp, element);
	}

	// 6. Set Input Image for hough transformation
	m_preprocessImg.houghImg = temp;

#endif
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
	cv::HoughLinesP(m_preprocessImg.houghImg, linesTemp, 1, CV_PI / 180, m_houghVar.minVotes, m_houghVar.minLineLength, m_houghVar.maxLineGap);
	m_houghVar.lines = linesTemp;


}

void LineDetector::detectObject()
{
	preprocImg();
#if !HAS_CUDA
	detectLines();
#endif
}
#if HAS_CUDA
cv::Mat LineDetector::getfinishedImg() const
{
	return finishedImg;
}
#endif
void LineDetector::drawLines(Mat& img, bool detectLanes, bool keepOnlyCertainAngles)
{
	int x_offset = getRoiBox().x;
	int y_offset = getRoiBox().y;


	float lane_1_m = 0.;
	float lane_2_m = 0.;
	int count_1 = 0;
	float lane_1_yc = 0.;
	float lane_2_yc = 0.;
	int count_2 = 0;
	Point roi_lane1_pt1, roi_lane1_pt2, roi_lane2_pt1, roi_lane2_pt2;

	for (auto it = m_houghVar.lines.begin(); it != m_houghVar.lines.end(); ++it) {
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
				if (m >= 0.39 & m <= 0.59) {
					lane_1_m += m;
					lane_1_yc += yc;
					roi_lane1_pt1.x += a.x;
					roi_lane1_pt1.y += a.y;
					roi_lane1_pt2.x += b.x;
					roi_lane1_pt2.y += b.y;
					count_1 += 1;
				}

				else if (m >= -0.7 & m <= -0.5) { //(m >= -0.75 & m <= -0.62)
					lane_2_m += m;
					lane_2_yc += yc;
					roi_lane2_pt1.x += a.x;
					roi_lane2_pt1.y += a.y;
					roi_lane2_pt2.x += b.x;
					roi_lane2_pt2.y += b.y;
					count_2 += 1;
				}
			}
			
			else if (keepOnlyCertainAngles) {
				float m = float(b.y - a.y) / float(b.x - a.x);
				float yc = b.y - m * a.y;
				if (m >= 0.39 & m <= 0.59) {
					cv::line(img, a, b, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
				}

				else if (m >= -0.7 & m <= -0.50) {
					cv::line(img, a, b, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
				}
			}

			else {
				cv::line(img, a, b, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
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
		dashboardLane1.y = getCurrImg().rows;
		dashboardLane2.y = getCurrImg().rows;


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


		bool goodLine = true;

		if ((dashboardLane1.x > 1100 & dashboardLane1.x < 1400) | (dashboardLane2.x > 1100 & dashboardLane2.x < 1400)) {
			goodLine = false;
		}

		if (count_1 > 0 & goodLine) {
			cv::arrowedLine(img, dashboardLane1, line1End, Scalar(0, 255, 0), m_houghVar.lineThickness, LINE_AA);
			m_houghVar.line1_pt1 = line1End;
			m_houghVar.line1_pt2 = dashboardLane1;
		}
		else {
			cv::arrowedLine(img, m_houghVar.line1_pt2, m_houghVar.line1_pt1, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
		}

		if (count_2 > 0 & goodLine) {
			cv::arrowedLine(img, dashboardLane2, line2End, Scalar(0, 255, 0), m_houghVar.lineThickness, LINE_AA);
			m_houghVar.line2_pt1 = line2End;
			m_houghVar.line2_pt2 = dashboardLane2;
		}
		else {
			cv::arrowedLine(img, m_houghVar.line2_pt2, m_houghVar.line2_pt1, Scalar(0, 0, 255), m_houghVar.lineThickness, LINE_AA);
		}
		//cout << avgMLane1 << " " << avgMLane2 << " " << m_houghVar.line1_pt1 << " " << m_houghVar.line1_pt2 << " " << m_houghVar.line2_pt1 << " " << m_houghVar.line2_pt2 << endl;
	}

}


// Setters & Getters

void LineDetector::setLines(const std::vector<cv::Vec4i>& lines_)
{
	m_houghVar.lines = lines_;
}

LineDetector::preprocessParams LineDetector::getPreprocessParams()
{
	return m_preprocessVar;
}

LineDetector::houghParams LineDetector::getHoughParams()
{
	return m_houghVar;
}



// Parameter initialisation

void LineDetector::setParams(const preprocessParams& pParams, const houghParams& hParams, const cv::Rect& roi_Bbox)
{
	m_houghVar = hParams;
	m_preprocessVar = pParams;
	ObjectDetector::setRoiBox(roi_Bbox);

#if HAS_CUDA
	m_HoughDetector = cv::cuda::createHoughSegmentDetector(1, CV_PI / 180, hParams.minLineLength, hParams.maxLineGap);
	m_CannyDetector = cv::cuda::createCannyEdgeDetector(pParams.cannyLowThresh, pParams.cannyHighTresh, pParams.cannyKernelSize + 3);
	m_BoxFilter = cv::cuda::createBoxFilter(CV_8U, CV_8U, cv::Size(pParams.gaussKernelSize, pParams.gaussKernelSize), cv::Point(-1, -1));

	Mat str_element = getStructuringElement(MORPH_RECT, Size(2 * m_preprocessVar.morphKernelSize1 + 1, 2 * m_preprocessVar.morphKernelSize1 + 1), Point(m_preprocessVar.morphKernelSize1, m_preprocessVar.morphKernelSize1));
	m_morphFilter_1 = cuda::createMorphologyFilter(m_preprocessVar.morphTransformType1, CV_8U, str_element, cv::Size(pParams.morphKernelSize1, pParams.morphKernelSize1));
	str_element = getStructuringElement(MORPH_RECT, Size(2 * m_preprocessVar.morphKernelSize2 + 1, 2 * m_preprocessVar.morphKernelSize2 + 1), Point(m_preprocessVar.morphKernelSize2, m_preprocessVar.morphKernelSize2));
	m_morphFilter_2 = cuda::createMorphologyFilter(m_preprocessVar.morphTransformType2, CV_8U, str_element, cv::Size(pParams.morphKernelSize2, pParams.morphKernelSize2));

	

#endif
}
