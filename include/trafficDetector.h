#pragma once
#ifndef trafficDetector__H__
#define trafficDetector__H__

#include <objectDetector.h>

class TrafficDetector : public ObjectDetector {

public:

	TrafficDetector(int iD);
	TrafficDetector();
	~TrafficDetector();
	void setDnnNet(cv::dnn::Net net);
	cv::dnn::Net getDnnNet();
	void detectObject();
	void detectObject(std::vector<cv::Rect2d>& trackBoxVec, std::mutex& mt_trackbox);
	std::string getModelTxt();
	std::string getModel();
	std::string getClasses();
	int getFramesUntilDetection();
	int getFailureCounter();
	int getTrackStatus();
	cv::Rect2d getTrackbox();
	int getCountsSinceLastSearch();
	void setFramesUntilDetection(int framesUntilDetec);
	void setFailureCounter(int failCount);
	void setCountsSinceLastSearch(int countsSince);


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
	



};
#endif 