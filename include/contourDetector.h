#pragma once
#ifndef contourDetector__H__
#define contourDetector__H__

#include <lineDetector.h>

class ContourDetector : public LineDetector {


public:
	void detectObject();
	
	cv::Mat getShowImg();

	void setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox);





private:
	preprocessParams preprocessVar;
	preprocessImgs preprocessImg;
	houghParams houghVar;
	std::vector<std::vector<cv::Point>> contours;
	double epsilon;
	int id;
	cv::Mat showImg;









};
#endif 