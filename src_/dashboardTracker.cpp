#include <dashboardTracker.h>
using namespace cv;
using namespace std;

DashboardTracker::DashboardTracker(int iD, LineDetector& lD, TrafficDetector& tD, CarTracker& cT) {
	id = iD;
	ld = lD;
	td = tD;
	ct = cT;
	ld.setId(iD);
	td.setId(iD);
	ct.setId(iD);
}

DashboardTracker::DashboardTracker()
{
}

DashboardTracker::~DashboardTracker()
{
}



int DashboardTracker::getId()
{
	return id;
}

void DashboardTracker::setLd(LineDetector lD)
{
	ld = lD;
}

void DashboardTracker::setCt(CarTracker cT)
{
	ct = cT;
}

void DashboardTracker::setTd(TrafficDetector tD)
{
	td = tD;
}

void DashboardTracker::setCurrImg(cv::Mat curr_Img)
{
	currImg = curr_Img.clone();
	ld.setCurrImg(curr_Img);
	td.setCurrImg(curr_Img);
	ct.setCurrImg(curr_Img);
}

cv::Mat DashboardTracker::getCurrImg()
{
	return currImg;
}

LineDetector DashboardTracker::getLd()
{
	return ld;
}

TrafficDetector DashboardTracker::getTd()
{
	return td;
}

CarTracker DashboardTracker::getCt()
{
	return ct;
}

void DashboardTracker::setImgProcessed(bool status)
{
	imgProcessed = status;
}

bool DashboardTracker::getImgProcessed()
{
	return imgProcessed;
}

std::thread DashboardTracker::dashboardThread(std::vector<bool>& imgAvailable, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int> &trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex& imgAvailableGuard, std::mutex& trackingStatusGuard, std::mutex& mt_trackbox, std::mutex& lines_reserve) {

	return std::thread(&DashboardTracker::runThread, this, std::ref(imgAvailable), std::ref(stopThreads), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailableGuard), std::ref(trackingStatusGuard), std::ref(mt_trackbox), std::ref(lines_reserve));
}



void DashboardTracker::runThread(std::vector<bool>& imgAvail, std::atomic<bool>& stopThreads, std::vector<cv::Rect2d>& trackBoxVec, std::vector<int>& trackingStatus, std::vector<std::vector<cv::Vec4i>>& lines, std::mutex & imgAvailableGuard, std::mutex & trackingStatusGuard, std::mutex & mt_trackbox, std::mutex & lines_reserve)
{
	// Reusable variables
	int trackStatus;
	Rect2d trackBox;
	Rect2d trackBox_empty;
	Rect2d roi_tracker_box = td.getRoiBox();
	bool updateSuccess;
	bool imgAvailable;
	td.setId(this->id);
	ld.setId(this->id);
	ct.setId(this->id);
	std::string CLASSES[21] = { "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
									"dog", "horse", "motorbike", "person", "pottedplant", "sheep","sofa", "train", "tvmonitor" };
	int countsSinceLastSearch = 20;
	int failureCounter = 0;


	while (true) {

		if (stopThreads) {
			break;
			return;
		}
		imgAvailable = false;
		{
			const std::lock_guard<mutex> lock(imgAvailableGuard);
			imgAvailable = imgAvail[id];
		}

		if (imgAvailable) {
			Mat img;
			{
				const std::lock_guard<mutex> lock(imgAvailableGuard);
				imgProcessed = false;
				imgAvail[id] = false;
				img = currImg.clone();
			}

			// Step 1: Lane Detection
			ld.detectObject();

			{
				const std::lock_guard<mutex> lock(lines_reserve);
				lines[id] = ld.getHoughParams().lines;
				trackStatus = trackingStatus[id];
			}

			// Optional Step 2: If no tracker instantiated, keep detecting
			if (trackStatus == 0 & countsSinceLastSearch >= 20)
			{
				countsSinceLastSearch = 0;
				{
					const std::lock_guard<mutex> lock(mt_trackbox);
					td.detectObject(trackBoxVec);
				}
				trackStatus = td.getTrackStatus();
				if (trackStatus == 1) {
					trackBox = td.getTrackbox();
					ct.initTracker(img(roi_tracker_box), trackBox);
					{
						const std::lock_guard<mutex> lock(trackingStatusGuard);
						trackingStatus[id] = 1;
						trackStatus = 1;
					}
				}
			}

			else if ((trackStatus == 1) | (trackStatus == 2)) { // DO NOT LET IT SET A NEW TRACKER!! IT UPDATES ON A DIFFERENT AREA AFTER TRACKER LOST??

				{
					const std::lock_guard<mutex> lock(mt_trackbox);
					updateSuccess = ct.updateTracker(img(roi_tracker_box), trackBoxVec[id]);
					bool trackBoxOk = true;
					for (int m = 0; m < trackBoxVec.size(); m++) {
						Rect2d trackBox_m = trackBoxVec[m];
						Point center_of_rect_m = (trackBox_m.br() + trackBox_m.tl())*0.5;
						if (center_of_rect_m.x != 0 & center_of_rect_m.y != 0) {
							for (int n = 0; n < trackBoxVec.size(); n++) {
								if (m != n) {
									Rect2d trackBox_n = trackBoxVec[n];
									Point center_of_rect_n = (trackBox_n.br() + trackBox_n.tl())*0.5;
									if (center_of_rect_n.x != 0 & center_of_rect_n.y != 0) {
										Point diff = center_of_rect_m - center_of_rect_n;
										float dist_Bboxes = cv::sqrt(diff.x*diff.x + diff.y*diff.y);

										// Threshold for another tracked object should be the distance between current bounding box centers
										if (dist_Bboxes < 100) {
											updateSuccess = false;
											trackBoxVec[id] = trackBox_empty;
											break;

										}
									}
								}
							}
						}
					}
				}


				if (updateSuccess)
				{
					failureCounter = 0;
					if (trackStatus == 2) {
						{
							const std::lock_guard<mutex> lock(trackingStatusGuard);
							trackingStatus[id] = 1;
						}
					}
				}

				else {
					failureCounter++;
					if (failureCounter > 5) {
						//putText(frame, "Tracking abandoned: too many frames without successful tracking", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
						{
							const std::lock_guard<mutex> lock(mt_trackbox);
							trackStatus = 0;
							trackingStatus[id] = trackStatus;
							trackBoxVec[id] = trackBox_empty;
						}
					}

					else {
						{
							const std::lock_guard<mutex> lock(trackingStatusGuard);
							trackingStatus[id] = 2;
						}
					}
				}
			}
		}

		else {
			countsSinceLastSearch++;
		}
		imgProcessed = true;
	}
	this_thread::sleep_for(chrono::milliseconds(10));
}







/*
void DashboardTracker::dashboardTrackersThread(DashboardTracker& aD, vector<bool>& imgAvail, atomic<bool>& stopThreads, vector<Rect2d>& trackBoxVec, vector<int> &trackingStatus, vector<vector<Vec4i>>& lines, mutex& imgAvailableGuard, mutex& trackingStatusGuard, mutex& mt_trackbox, mutex& lines_reserve) {

	// Reusable variables
	int id = aD.getId();
	int trackStatus;
	Rect2d trackBox;
	Rect2d trackBox_empty;
	bool updateSuccess;
	bool imgAvailable;
	LineDetector ld_;
	CarTracker ct_;
	TrafficDetector td_ = aD.getTd();
	Rect2d roi_tracker_box = td_.getRoiBox();
	//const std::string CLASSES = *aD.getTd().getClasses();
	std::string CLASSES[21] = { "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
									"dog", "horse", "motorbike", "person", "pottedplant", "sheep","sofa", "train", "tvmonitor" };
	int countsSinceLastSearch = 20;
	int failureCounter = 0;


	while (true) {

		if (stopThreads) {
			break;
			return;
		}
		imgAvailable = false;
		{
			const std::lock_guard<mutex> lock(imgAvailableGuard);
			imgAvailable = imgAvail[id];
		}

		if (imgAvailable) {
			Mat img;
			{
				const std::lock_guard<mutex> lock(imgAvailableGuard);
				aD.setImgProcessed(false);
				imgAvail[id] = false;
				img = aD.getCurrImg().clone();
			}

			// Step 1: Lane Detection
			ld_ = aD.getLd();
			ld_.detectObject();

			{
				const std::lock_guard<mutex> lock(lines_reserve);
				lines[id] = ld_.getHoughParams().lines;
				trackStatus = trackingStatus[id];
			}

			// Optional Step 2: If no tracker instantiated, keep detecting

			if (trackStatus == 0 & countsSinceLastSearch >= 20)
			{
				countsSinceLastSearch = 0;
				td_ = aD.getTd();
				td_.detectObject(trackBoxVec, mt_trackbox);
				trackStatus = td_.getTrackStatus();
				aD.setTd(td_);
				if (trackStatus == 1) {
					ct_ = aD.getCt();
					trackBox = td_.getTrackbox();
					ct_.initTracker(img(roi_tracker_box), trackBox);
					aD.setCt(ct_);
					{
						const std::lock_guard<mutex> lock(trackingStatusGuard);
						trackingStatus[id] = 1;
						trackStatus = 1;
					}
				}
			}

			else if ((trackStatus == 1) | (trackStatus == 2)){ // DO NOT LET IT SET A NEW TRACKER!! IT UPDATES ON A DIFFERENT AREA AFTER TRACKER LOST??
				ct_ = aD.getCt();

				{
					const std::lock_guard<mutex> lock(mt_trackbox);
					updateSuccess = ct_.updateTracker(img(roi_tracker_box), trackBoxVec[id]);
				}
				aD.setCt(ct_);
				if (updateSuccess)
				{
					failureCounter = 0;
					if (trackStatus == 2) {
						{
							const std::lock_guard<mutex> lock(trackingStatusGuard);
							trackingStatus[id] = 1;
						}
					}
				}

				else {
					failureCounter++;
					if (failureCounter > 5) {
						//putText(frame, "Tracking abandoned: too many frames without successful tracking", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
						{
							const std::lock_guard<mutex> lock(mt_trackbox);
							trackStatus = 0;
							trackingStatus[id] = trackStatus;
							trackBoxVec[id] = trackBox_empty;
						}
					}
					else {
						{
							const std::lock_guard<mutex> lock(trackingStatusGuard);
							trackingStatus[id] = 2;
						}
					}
				}
			}

			else {
				countsSinceLastSearch++;
			}
			aD.setImgProcessed(true);
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}

}
*/

void DashboardTracker::setId(int iD)
{
	id = iD;
	ld.setId(id);
	td.setId(id);
	ct.setId(id);
}



