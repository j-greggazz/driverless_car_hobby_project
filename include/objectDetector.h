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


class ObjectDetector {


public:
	
	// Constructor / Destructor
	ObjectDetector();
	ObjectDetector(int iD);
	virtual ~ObjectDetector();

	// Setter Methods
	void setId(int iD);
	void setCurrImg(cv::Mat& curr_Img);
	void setRoiBox(cv::Rect roi_Bbox_); // get methods to access all params and set RoiBox to these
	void setImgProcessed(bool img_processed);

	// Getter Methods
	int getId();
	cv::Mat getCurrImg();
	bool getImgProcessed();
	cv::Rect getRoiBox(); // get methods to access all params and set RoiBox to these

	// Virtual function(s)
	virtual void detectObject() = 0;

private:
	// Member Variables
	int id;
	cv::Rect roi_Bbox;// = cv::Rect(x1_roi, y1_roi, recWidth, recHeight);
	cv::Mat currImg;
	bool imgProcessed;
};
#endif 