#pragma once
#ifndef contourDetector__H__
#define contourDetector__H__

#include <lineDetector.h>

class ContourDetector : public LineDetector {


public:
	void detectObject();
	
	cv::Mat getShowImg() const;

	void setParams(const preprocessParams& pParams, const houghParams& hParams, const cv::Rect& roi_Bbox);





private:
	preprocessParams m_preprocessVar;
	preprocessImgs m_preprocessImg;
	houghParams m_houghVar;
	std::vector<std::vector<cv::Point>> m_contours;
	double m_epsilon;
	int m_id;
	cv::Mat m_showImg;









};
#endif 