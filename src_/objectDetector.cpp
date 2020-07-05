#include <objectDetector.h>

using namespace cv;
using namespace std;

// Constructors / Destructor
ObjectDetector::ObjectDetector()
{
}

ObjectDetector::ObjectDetector(int iD) {
	id = iD;
}

ObjectDetector:: ~ObjectDetector() {};


// Public Methods - getters & setters

// - Getters
int ObjectDetector::getId() {
	return id;
}
cv::Mat ObjectDetector::getCurrImg()
{
	return currImg;
}


cv::Rect ObjectDetector::getRoiBox() {
	return roi_Bbox;
}

bool ObjectDetector::getImgProcessed() {
	return imgProcessed;
}

// - Setters
void ObjectDetector::setId(int iD) {
	id = iD;
}

void ObjectDetector::setCurrImg(cv::Mat& curr_Img) {
	currImg = curr_Img.clone();
}


void ObjectDetector::setRoiBox(cv::Rect roi_Bbox_) {
	roi_Bbox = roi_Bbox_;
}

void ObjectDetector::setImgProcessed(bool img_processed) {
	imgProcessed = img_processed;
}



