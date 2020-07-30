#pragma once
#ifndef trafficDetector__H__
#define trafficDetector__H__

#include <objectDetector.h>

class TrafficDetector : public ObjectDetector {

public:

    // Base class constructor-destructor used 

	// Getters & setters:
	void setDnnNet(cv::dnn::Net net);
	cv::dnn::Net getDnnNet();
	void detectObject();
	void setModelTxt(std::string);
	void setModelBin(std::string);
	void detectObject(std::vector<cv::Rect2d>& trackBoxVec, std::mutex& mt_trackbox);
	std::string getModelTxt();
	std::string getModel();
	std::string* getClasses();
	int getTrackStatus();
	cv::Rect2d getTrackbox();
	void setTrackerLabel(std::string trackerLabel);
	std::string getTrackerLabel();
	std::vector<cv::Rect2d> getTrackBoxVec();

private:
	cv::dnn::Net dnnNet;
	bool detect;
	std::string CLASSES[21] = { "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
									"dog", "horse", "motorbike", "person", "pottedplant", "sheep","sofa", "train", "tvmonitor" };
	int framesUntilDetection;
	int failCounter;
	int trackingStatus; // If 0 no tracker instantiated, if 1 tracker instantiated and tracking, if 2 tracker instantiated however has lost initial tracked object 
	int countsSinceLastSearch;
	std::string modelTxt = "../models/MobileNetSSD_deploy.prototxt.txt";
	std::string modelBin = "../models/MobileNetSSD_deploy.caffemodel";
	cv::Rect2d td_trackbox;
	std::string tracker_label;
	int id;
	std::vector<cv::Rect2d> m_trackBoxVec;

};
#endif 