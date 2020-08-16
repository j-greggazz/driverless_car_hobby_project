#pragma once
#ifndef objectDetector__H__
#define objectDetector__H__

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <atomic>
#include <vector>
#include <iostream>
#include <mutex>
#define HAS_CUDA 1

class ObjectDetector {


public:
	
	// Constructor / Destructor
	ObjectDetector();
	ObjectDetector(const int& iD);
	virtual ~ObjectDetector();

	// Setter Methods
	void setId(const int& iD);
	void setCurrImg(const cv::Mat& curr_Img);
	void setRoiBox(const cv::Rect& roi_Bbox_); // get methods to access all params and set RoiBox to these
	void setImgProcessed(const bool& img_processed);

	// Getter Methods
	int getId() const;
	cv::Mat getCurrImg() const;
	bool getImgProcessed() const;
	cv::Rect getRoiBox() const; // get methods to access all params and set RoiBox to these

	// Virtual function(s)
	virtual void detectObject() = 0;

private:
	// Member Variables
	int m_id;
	cv::Rect m_roi_Bbox;// = cv::Rect(x1_roi, y1_roi, recWidth, recHeight);
	cv::Mat m_currImg;
	bool m_imgProcessed;
};
#endif 