#include <objectDetector.h>

using namespace cv;
using namespace std;

// Constructors / Destructor
ObjectDetector::ObjectDetector()
{
}

ObjectDetector::ObjectDetector(int iD) {
	m_id = iD;
}

ObjectDetector:: ~ObjectDetector() {};


// Public Methods - getters & setters

// - Getters
int ObjectDetector::getId() {
	return m_id;
}
cv::Mat ObjectDetector::getCurrImg()
{
	return m_currImg;
}


cv::Rect ObjectDetector::getRoiBox() {
	return m_roi_Bbox;
}

bool ObjectDetector::getImgProcessed() {
	return m_imgProcessed;
}

// - Setters
void ObjectDetector::setId(int iD) {
	m_id = iD;
}

void ObjectDetector::setCurrImg(cv::Mat& curr_Img) {
	m_currImg = curr_Img.clone();
}


void ObjectDetector::setRoiBox(cv::Rect roi_Bbox_) {
	m_roi_Bbox = roi_Bbox_;
}

void ObjectDetector::setImgProcessed(bool img_processed) {
	m_imgProcessed = img_processed;
}



