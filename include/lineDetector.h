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

	// Setter Methods
	//void setProcessingParams(struct )
	void setCannyKernelSize(int canny_KernelSize);
	void setCannyLowThresh(int canny_LowThresh);
	void setCannyHighThresh(int canny_HighThresh);
	void setGaussKernelSize(int gauss_KernelSize);
	void setMorphElemShape(int morph_ElemShape);
	void setMorphTransformType1(int morph_TransformType1);
	void setMorphTransformType2(int morph_TransformType2);
	void setMorphKernelSize1(int morph_KernelSize1);
	void setMorphKernelSize2(int morph_KernelSize2);
	preprocessParams getPreprocessParams();
	houghParams getHoughParams();

	void setRoiImg(cv::Mat& roi_Img);
	void setBlurImg(cv::Mat& blur_Img);
	void setGrayImg(cv::Mat& gray_Img);
	void setCannyImg(cv::Mat& canny_Img);
	void setMorphImg(cv::Mat& morph_Img);
	void setHoughImg(cv::Mat& hough_Img);


	// Getter Methods
	int getCannyKernelSize();
	int getCannyLowThresh();
	int getCannyHighThresh();
	int getGaussKernelSize();
	int getMorphElemShape();
	int getMorphTransformType1();
	int getMorphTransformType2();
	int getMorphKernelSize1();
	int getMorphKernelSize2();

	cv::Mat getRoiImg();
	cv::Mat getBlurImg();
	cv::Mat getGrayImg();
	cv::Mat getCannyImg();
	cv::Mat getMorphImg();
	cv::Mat getHoughImg();


	// Class Specific Methods
	void preprocImg();
	void detectLines();

	// Implement Base-Class Virtual Method(s)
	void detectObject();

	static void drawLines(LineDetector& ld, bool detectLanes = true);

	void setParams(preprocessParams pParams, houghParams hParams, cv::Rect roi_Bbox);


private:

	preprocessParams preprocessVar;
	preprocessImgs preprocessImg;
	houghParams houghVar;

};
#endif 