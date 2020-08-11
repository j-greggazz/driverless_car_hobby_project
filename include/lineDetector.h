#pragma once
#ifndef lineDetector__H__
#define lineDetector__H__

#include <objectDetector.h>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>

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
		cv::Mat houghImg;
	};

	// Hough Line Detection Parameters
	struct houghParams {
		int minVotes;
		int minLineLength;
		int maxLineGap;
		int lineThickness = 2;
		std::vector<cv::Vec4i> lines;
		cv::Point line1_pt1, line1_pt2, line2_pt1, line2_pt2; // lane dimensions
	};

	preprocessParams getPreprocessParams();
	houghParams getHoughParams();

	// Class Specific Methods
	void preprocImg();
	void detectLines();
	void drawLines(cv::Mat& img, bool detectLanes = true, bool keepOnlyCertainAngles = false);

	// Getters - Setters
	void setLines(std::vector<cv::Vec4i> lines_);
	void setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox);

	// Implement virtual function of base class
	void detectObject();

	// Cuda Methods
	cv::Mat getfinishedImg();

	
private:

	preprocessParams m_preprocessVar;
	preprocessImgs m_preprocessImg;
	houghParams m_houghVar;
	int id;

#if HAS_CUDA
	cv::Ptr<cv::cuda::HoughSegmentDetector> m_HoughDetector;
	cv::Ptr<cv::cuda::CannyEdgeDetector> m_CannyDetector;
	cv::Ptr<cv::cuda::Filter> m_BoxFilter;
	cv::Ptr<cv::cuda::Filter> m_morphFilter_1;
	cv::Ptr<cv::cuda::Filter> m_morphFilter_2;
	std::vector<cv::Vec4i> m_lines;
	cv::Mat finishedImg;
	
#endif	

};
#endif 