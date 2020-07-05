#include <autoDrive.h>
using namespace cv;
using namespace std;

AutoDrive::AutoDrive(int iD, LineDetector& lD, TrafficDetector& tD, CarTracker& cT) {
	id = iD;
	ld = lD;
	td = tD;
	ct = cT;
	ld.setId(iD);
	td.setId(iD);
	ct.setId(iD);
}

AutoDrive::AutoDrive()
{
}

AutoDrive::~AutoDrive()
{
}



int AutoDrive::getId()
{
	return id;
}

void AutoDrive::setLd(LineDetector lD)
{
	ld = lD;
}

void AutoDrive::setCt(CarTracker cT)
{
	ct = cT;
}

void AutoDrive::setTd(TrafficDetector tD)
{
	td = tD;
}

void AutoDrive::setCurrImg(cv::Mat curr_Img)
{
	currImg = curr_Img.clone();
	ld.setCurrImg(curr_Img);
	td.setCurrImg(curr_Img);
	ct.setCurrImg(curr_Img);
}

cv::Mat AutoDrive::getCurrImg()
{
	return currImg;
}

LineDetector AutoDrive::getLd()
{
	return ld;
}

TrafficDetector AutoDrive::getTd()
{
	return td;
}

CarTracker AutoDrive::getCt()
{
	return ct;
}

void AutoDrive::setImgProcessed(bool status)
{
	imgProcessed = status;
}

bool AutoDrive::getImgProcessed()
{
	return imgProcessed;
}

void AutoDrive::autoDriveThread(AutoDrive& aD, vector<bool>& imgAvail, atomic<bool>& stopThreads, vector<Rect2d>& trackBoxVec, vector<int> &trackingStatus, vector<vector<Vec4i>>& lines, mutex& imgAvailableGuard, mutex& trackingStatusGuard, mutex& mt_trackbox, mutex& lines_reserve) {

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


void AutoDrive::setId(int iD)
{
	id = iD;
	ld.setId(id);
	td.setId(id);
	ct.setId(id);
}



