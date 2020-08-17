#pragma once
#ifndef dashboardTracker__H__
#define dashboardTracker__H__
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

class DashboardTracker {

public:
	DashboardTracker(const int& iD, const LineDetector& lD, const TrafficDetector& tD, const CarTracker& cT);
	DashboardTracker();
	~DashboardTracker();

	void setId(const int& iD);
	void setLd(const LineDetector& lD);
	void setCt(const CarTracker& cT);
	void setTd(const TrafficDetector& tD);
	void setCurrImg(const cv::Mat& curr_Img);
	void setImgProcessed(const bool& status);

	int getId() const;
	cv::Mat getCurrImg() const;
	LineDetector getLd() const;
	TrafficDetector getTd() const;
	CarTracker getCt() const;
	bool getImgProcessed() const;
	
	// Threading Methods
	// 1. Object Method
	void runThread(std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);
	std::thread dashboardThread(std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);
	
	// 2. Static Method
	static void staticMethodThread(DashboardTracker& dT, std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);


	//static void dashboardTrackersThread(DashboardTracker& aD, std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve);

private:

	LineDetector m_ld;
	TrafficDetector m_td;
	CarTracker m_ct;
	cv::Mat m_currImg;
	int m_id;
	bool m_imgProcessed = false;












};
#endif 