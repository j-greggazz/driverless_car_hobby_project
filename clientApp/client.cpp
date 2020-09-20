#include <carTracker.h>


using namespace cv;
using namespace std;

void CarTracker::declareTracker(const string& trackerType)
{
	if (trackerType == "BOOSTING")
		m_tracker = TrackerBoosting::create();
	if (trackerType == "MIL")
		m_tracker = TrackerMIL::create();
	if (trackerType == "KCF")
		m_tracker = TrackerKCF::create();
	if (trackerType == "TLD")
		m_tracker = TrackerTLD::create();
	if (trackerType == "MEDIANFLOW")
		m_tracker = TrackerMedianFlow::create();
	if (trackerType == "GOTURN")
		m_tracker = TrackerGOTURN::create();
	if (trackerType == "MOSSE")
		m_tracker = TrackerMOSSE::create();
	if (trackerType == "CSRT")
		m_tracker = TrackerCSRT::create();
}

void CarTracker::initTracker(const cv::Mat& frame, const cv::Rect2d& trackBox)
{
	m_tracker->init(frame, trackBox);
	m_trackerExists = true;
}

bool CarTracker::updateTracker(const cv::Mat& frame, cv::Rect2d& trackBox)
{
	return m_tracker->update(frame, trackBox);
}

void CarTracker::setId(const int& iD)
{
	m_id = iD;
}

void CarTracker::setCurrImg(const cv::Mat& curr_Img)
{
	m_currImg = curr_Img;
}

cv::Mat CarTracker::getCurrImg() const
{
	return m_currImg;
}

std::string CarTracker::getTrackerType() const
{
	return m_trackerType;
}

std::vector<cv::Ptr<cv::Tracker>> CarTracker::getTrackersVec() const
{
	return m_trackersVec;
}

void CarTracker::setTrackersVec(const std::vector<cv::Ptr<cv::Tracker>>& newTrackersVec)
{
	m_trackersVec = newTrackersVec;
}

cv::Ptr<cv::Tracker> CarTracker::getTracker() const
{
	return m_tracker;
}

void CarTracker::setTracker(const cv::Ptr<cv::Tracker>& tracker_)
{
	m_tracker = tracker_;
}

void CarTracker::setTrackerType(const std::string& tracker_type)
{
	m_trackerType = tracker_type;
}


