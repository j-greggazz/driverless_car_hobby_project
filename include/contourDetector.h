#pragma once
#ifndef contourDetector__H__
#define contourDetector__H__

#include <lineDetector.h>

class ContourDetector : public LineDetector {


public:
	void detectObject();
	








private:
	preprocessParams preprocessVar;
	preprocessImgs preprocessImg;
	houghParams houghVar;
	std::vector<std::vector<cv::Point>> contours;
	cv::Rect roi_Bbox;
	double epsilon;
	int id;









};
#endif 