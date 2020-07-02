#pragma once
#ifndef autoDrive__H__
#define autoDrive__H__
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
#include <lineDetector.h>
#include <trafficDetector.h>
#include <carTracker.h>

class AutoDrive {

public:
	AutoDrive(int iD, LineDetector lD, TrafficDetector tD, CarTracker cT);
	~AutoDrive();
	//void updateParams();
	void updateImg(cv::Mat& img);
	static void autoDriveThread(AutoDrive& aD, std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);
	void setId(int iD);
	int getId();
	LineDetector getLd();
	TrafficDetector getTd();
	CarTracker getCt();
	void setImgProcessed(bool status);
	bool getImgProcessed();

private:

	LineDetector ld;
	TrafficDetector td;
	CarTracker ct;
	int id;
	bool imgProcessed = false; 











};
#endif 