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

void AutoDrive::updateImg(cv::Mat& img)
{
	ld.setCurrImg(img);
	td.setCurrImg(img);
	ct.setCurrImg(img);
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
	//void AutoDrive::autoDriveThread(AutoDrive& aD, atomic<bool>& imgAvail, atomic<bool>& stopThreads, vector<Rect2d>& trackBoxVec, vector<int> &trackingStatus, vector<vector<Vec4i>>& lines, mutex& mt_trackbox, mutex& lines_reserve) {


	int id = aD.getId();
	int trackStatus;
	Rect2d trackBox;
	//const std::string CLASSES = *aD.getTd().getClasses();
	std::string CLASSES[21] = { "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
									"dog", "horse", "motorbike", "person", "pottedplant", "sheep","sofa", "train", "tvmonitor" };
	TrafficDetector td_ = aD.getTd();
	td_.setCountsSinceLastSearch(30);
	aD.setTd(td_);
	//aD.getTd().setCountsSinceLastSearch(30);
	//dnn::Net net = aD.getTd().getDnnNet();

	while (true) {

		if (stopThreads) {
			break;
			return;
		}
		bool imgAvailable = false;
		{
			const std::lock_guard<mutex> lock(imgAvailableGuard);
			imgAvailable = imgAvail[id];
		}

		if (imgAvailable) {
			Mat img;
			{
				const std::lock_guard<mutex> lock(imgAvailableGuard);
				imgAvail[id] = false;
				img = aD.getCurrImg();
			}

			// Step 1: Lane Detection
			aD.getLd().detectObject();

			{
				const std::lock_guard<mutex> lock(lines_reserve);
				lines[id] = aD.getLd().getHoughParams().lines;
				trackStatus = trackingStatus[id];
				
			}

			// Optional Step 2: If no tracker instantiated, keep detecting
			if (trackStatus == 0 & aD.getTd().getCountsSinceLastSearch() >= 30)  // No object currently tracked and detection has not been run for 40 frames
			{	
				TrafficDetector td_ = aD.getTd();
				td_.setCountsSinceLastSearch(0);
				aD.setTd(td_);
				aD.getTd().detectObject(trackBoxVec, mt_trackbox);
				trackStatus = aD.getTd().getTrackStatus();

				if (trackStatus == 1) {
					Rect2d trackbox = aD.getTd().getTrackbox();
					aD.getCt().initTracker(img, trackBox);
				}
			}

			else if (trackStatus == 1) {
				bool updateSuccess = aD.getCt().updateTracker(img, trackBox);
				if (updateSuccess)
				{
					// Tracking success : Draw the tracked object
					aD.getTd().setFailureCounter(0);
				}

				else {
					int currFailCount = aD.getTd().getFailureCounter() + 1;
					aD.getTd().setFailureCounter(currFailCount);
					if (currFailCount > 30) {
						//putText(frame, "Tracking abandoned: too many frames without successful tracking", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
						{
							const std::lock_guard<mutex> lock(mt_trackbox);
							trackStatus = 0;
							trackingStatus[id] = trackStatus;
							Rect2d trackBox;
							trackBoxVec[id] = trackBox;
						}
					}
				}
			}

			else {
				int counts = aD.getTd().getCountsSinceLastSearch() + 1;
				aD.getTd().setCountsSinceLastSearch(counts);
			}
			aD.setImgProcessed(true);
		}

		//waitKey(100);
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



