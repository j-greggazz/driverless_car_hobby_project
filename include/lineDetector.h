#pragma once
#ifndef lineDetector__H__
#define lineDetector__H__

#include <objectDetector.h>

class LineDetector : public ObjectDetector {

public:

	// Preprocessing structs:
	struct preprocessParams {
		int cannyKernelSize;
		int cannyLowThresh;
		int cannyHighTresh;
		// - Blur
		int gaussKernelSize;
		// - Morphological Operations (General)
		int morphElemShape;
		// - Morphological Operation 1
		int morphTransformType1;
		int morphKernelSize1;
		// - Morphological Operation 2
		int morphTransformType2;
		int morphKernelSize2;
	};

	struct preprocessImgs {
		cv::Mat roiImg;
		cv::Mat blurImg;
		cv::Mat grayImg;
		cv::Mat cannyImg;
		cv::Mat strElement;
		cv::Mat morphImg;
		cv::Mat houghImg;
	};


	// Hough Line Detection Parameters
	struct houghParams {
		int minVotes;
		int minLineLength;
		int maxLineGap;
		int lineThickness;
		std::vector<cv::Vec4i> lines;
		cv::Point line1_pt1, line1_pt2, line2_pt1, line2_pt2; // lane dimensions
	};

	LineDetector(int iD);
	LineDetector();
	~LineDetector();



	preprocessParams getPreprocessParams();
	houghParams getHoughParams();


	//cv::Mat getCurrImg();

	// Class Specific Methods
	void preprocImg();
	void detectLines();
	void setLines(std::vector<cv::Vec4i> lines_);
	// Implement Base-Class Virtual Method(s)
	void detectObject();

	static void drawLines(LineDetector& ld, cv::Mat& img, bool detectLanes = true);

	void setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox);


private:

	preprocessParams preprocessVar;
	preprocessImgs preprocessImg;
	houghParams houghVar;
	//cv::Mat currImg;

};
#endif 