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
	void setTracker(cv::Ptr<cv::Tracker> tracker_);
	bool updateTracker(cv::Mat& frame, cv::Rect2d& trackBox);
	void setId(int iD);
	void setCurrImg(cv::Mat& curr_Img);
	cv::Mat getCurrImg();
	std::vector<cv::Ptr<cv::Tracker>> getTrackersVec();
	void setTrackersVec(std::vector<cv::Ptr<cv::Tracker>> newTrackersVec);

private:

	cv::Ptr<cv::Tracker> m_tracker;
	bool m_trackerExists;
	int m_trackingStatus;
	int m_id;
	cv::Mat m_currImg;

	// Cuda variables:
	std::vector<cv::Ptr<cv::Tracker>> m_trackersVec;








};
#endif 