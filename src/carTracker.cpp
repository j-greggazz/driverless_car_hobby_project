#include <carTracker.h>


using namespace cv;
using namespace std;

void CarTracker::declareTracker(string trackerType)
{
	if (trackerType == "BOOSTING")
		tracker = TrackerBoosting::create();
	if (trackerType == "MIL")
		tracker = TrackerMIL::create();
	if (trackerType == "KCF")
		tracker = TrackerKCF::create();
	if (trackerType == "TLD")
		tracker = TrackerTLD::create();
	if (trackerType == "MEDIANFLOW")
		tracker = TrackerMedianFlow::create();
	if (trackerType == "GOTURN")
		tracker = TrackerGOTURN::create();
	if (trackerType == "MOSSE")
		tracker = TrackerMOSSE::create();
	if (trackerType == "CSRT")
		tracker = TrackerCSRT::create();
}

void CarTracker::initTracker(cv::Mat frame, cv::Rect2d trackBox)
{
	tracker->init(frame, trackBox);
	trackerExists = true;
}

bool CarTracker::updateTracker(cv::Mat& frame, cv::Rect2d& trackBox)
{
	return tracker->update(frame, trackBox);
}

void CarTracker::setId(int iD)
{
	id = iD;
}

void CarTracker::setCurrImg(cv::Mat & curr_Img)
{
	currImg = curr_Img.clone();
}

cv::Mat CarTracker::getCurrImg()
{
	return currImg;
}

cv::Ptr<cv::Tracker> CarTracker::getTracker()
{
	return tracker;
}


