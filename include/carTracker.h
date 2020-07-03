#pragma once
#ifndef carTracker__H__
#define carTracker__H__
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

class CarTracker {

public:
	void declareTracker(std::string trackerType);
	void initTracker(cv::Mat frame, cv::Rect2d trackbox);
	cv::Ptr<cv::Tracker> getTracker();
	bool updateTracker(cv::Mat& frame, cv::Rect2d& trackBox);
	void setId(int iD);
	void setCurrImg(cv::Mat& curr_Img);
	cv::Mat getCurrImg();

private:

	cv::Ptr<cv::Tracker> tracker;
	bool trackerExists;
	int trackingStatus;
	int id;
	cv::Mat currImg;








};
#endif 