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
#include <chrono>

class AutoDrive {

public:
	AutoDrive(int iD, LineDetector& lD, TrafficDetector& tD, CarTracker& cT);
	AutoDrive();
	~AutoDrive();
	//void updateParams();
	void updateImg(cv::Mat& img);
	static void autoDriveThread(AutoDrive& aD, std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);
	void setId(int iD);
	int getId();
	void setLd(LineDetector lD);
	void setCt(CarTracker cT);
	void setTd(TrafficDetector tD);
	void setCurrImg(cv::Mat curr_Img);
	cv::Mat getCurrImg();
	LineDetector getLd();
	TrafficDetector getTd();
	CarTracker getCt();
	void setImgProcessed(bool status);
	bool getImgProcessed();

private:

	LineDetector ld;
	TrafficDetector td;
	CarTracker ct;
	cv::Mat currImg;
	int id;
	bool imgProcessed = false; 











};
#endif 