#pragma once
#ifndef trafficDetector__H__
#define trafficDetector__H__

#include <objectDetector.h>

class TrafficDetector : public ObjectDetector {

public:

    // Base class constructor-destructor used 

	// Getters & setters:
	void setDnnNet(const cv::dnn::Net& net);
	void detectObject();
	void setModelTxt(const std::string&);
	void setModelBin(const std::string&);
	void setTrackStatus(const int& track_status);

	void detectObject(std::vector<cv::Rect2d>& trackBoxVec, std::mutex& mt_trackbox);
	cv::dnn::Net getDnnNet() const;
	std::string getModelTxt() const;
	std::string getModel() const;
	std::string* getClasses();
	int getTrackStatus() const;
	cv::Rect2d getTrackbox() const;
	std::string getTrackerLabel() const;
	std::vector<cv::Rect2d> getTrackBoxVec() const;

	void setTrackerLabel(const std::string& trackerLabel);
	
private:
	cv::dnn::Net m_dnnNet;
	//bool m_detect;
	std::string m_CLASSES[21] = { "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
									"dog", "horse", "motorbike", "person", "pottedplant", "sheep","sofa", "train", "tvmonitor" };
	int m_framesUntilDetection;
	int m_failCounter;
	int m_trackingStatus; // If 0 no tracker instantiated, if 1 tracker instantiated and tracking, if 2 tracker instantiated however has lost initial tracked object 
	int m_countsSinceLastSearch;
	std::string m_modelTxt = "../../models/MobileNetSSD_deploy.prototxt.txt";
	std::string m_modelBin = "../../models/MobileNetSSD_deploy.caffemodel";
	cv::Rect2d m_trackbox;
	std::string m_trackerLabel;
	std::vector<cv::Rect2d> m_trackBoxVec;

};
#endif 