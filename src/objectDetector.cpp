#include <objectDetector.h>

using namespace cv;
using namespace std;

// Constructors / Destructor
ObjectDetector::ObjectDetector()
{
}

ObjectDetector::ObjectDetector(const int& iD) {
	m_id = iD;
}

ObjectDetector:: ~ObjectDetector() {};


// Public Methods - getters & setters

// - Getters
int ObjectDetector::getId() const
{
	return m_id;
}
cv::Mat ObjectDetector::getCurrImg() const
{
	return m_currImg;
}


cv::Rect ObjectDetector::getRoiBox() const
{
	return m_roi_Bbox;
}

bool ObjectDetector::getImgProcessed()  const 
{
	return m_imgProcessed;
}

// - Setters
void ObjectDetector::setId(const int& iD) {
	m_id = iD;
}

void ObjectDetector::setCurrImg(const cv::Mat& curr_Img) {
	m_currImg = curr_Img;
}


void ObjectDetector::setRoiBox(const cv::Rect& roi_Bbox_) {
	m_roi_Bbox = roi_Bbox_;
}

void ObjectDetector::setImgProcessed(const bool& img_processed) {
	m_imgProcessed = img_processed;
}



