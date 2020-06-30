#include <objectDetector.h>

using namespace cv;
using namespace std;


ObjectDetector::ObjectDetector()
{
}

// Constructor / Destructor
ObjectDetector::ObjectDetector(int iD) {
	id = iD;
}
ObjectDetector:: ~ObjectDetector() {};




// Public Methods
// 1. Setters:
void ObjectDetector::setId(int iD) {
	id = iD;
}

void ObjectDetector::setCurrImg(cv::Mat& curr_Img) {
	currImg = curr_Img;
}

void ObjectDetector::set_x1_roi(int x1_Roi) {
	x1_roi = x1_Roi;
}

void ObjectDetector::set_y1_roi(int y1_Roi) {
	y1_roi = y1_Roi;
}

void ObjectDetector::setRecWidth(int rec_Width) {
	recWidth = rec_Width;
}

void ObjectDetector::setRecHeight(int rec_Height) {
	recHeight = rec_Height;
}

void ObjectDetector::setRoiBox(cv::Rect roi_Bbox_) {
	roi_Bbox = roi_Bbox_;
}

void ObjectDetector::setImgProcessed(bool img_processed) {
	imgProcessed = img_processed;
}
// 2. Getters:

int ObjectDetector::getId() {
	return id;
}
cv::Mat ObjectDetector::getCurrImg()
{
	return currImg;
}
;

int ObjectDetector::get_x1_roi() {
	return x1_roi;
};

int ObjectDetector::get_y1_roi() {
	return y1_roi;
};

int ObjectDetector::getRecWidth() {
	return recWidth;
};

int ObjectDetector::getRecHeight() {
	return recHeight;
};

cv::Rect ObjectDetector::getRoiBox() {
	return roi_Bbox;
}

bool ObjectDetector::getImgProcessed() {
	return imgProcessed;
}
