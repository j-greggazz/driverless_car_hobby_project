#include <autoDrive.h>
using namespace cv;
using namespace std;

AutoDrive::AutoDrive(int iD, LineDetector lD, TrafficDetector tD, CarTracker cT) {
	id = iD;
	ld = lD;
	td = tD;
	ct = cT;
	ld.setId(iD);
	td.setId(iD);
	ct.setId(iD);
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

void AutoDrive::autoDriveThread(AutoDrive& aD, atomic<bool>& imgAvail, atomic<bool>& stopThreads, vector<Rect2d>& trackBoxVec, vector<int> &trackingStatus, vector<vector<Vec4i>>& lines, mutex& mt_trackbox, mutex& lines_reserve) {


	int id = aD.getLd().getId();
	int trackStatus;
	Rect2d trackBox;
	const std::string CLASSES = aD.getTd().getClasses();
	//dnn::Net net = aD.getTd().getDnnNet();

	while (true) {

		if (stopThreads) {
			break;
			return;
		}

		else if (imgAvail) {
			Mat img;
			imgAvail = false;

			// Step 1: Lane Detection
			aD.getLd().detectObject();

			{
				const std::lock_guard<mutex> lock(lines_reserve);
				lines[id] = aD.getLd().getHoughParams().lines;
				trackStatus = trackingStatus[id];
				img = aD.getLd().getCurrImg();
			}

			// Step 2: 
			if (trackStatus == 0 & aD.getTd().getCountsSinceLastSearch() >= 30)  // No object currently tracked and detection has not been run for 40 frames
			{
				aD.getTd().setCountsSinceLastSearch(0);
				aD.getTd().detectObject(trackBoxVec, mt_trackbox);



			}
		}
	}
}
				/*
				Mat inputBlob = dnn::blobFromImage(img, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);
				net.setInput(inputBlob, "data");
				Mat detection = net.forward("detection_out");
				Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
				ostringstream ss;
				float confidenceThreshold = 0.1;
				for (int i = 0; i < detectionMat.rows; i++) {
					float confidence = detectionMat.at<float>(i, 2);

					if (confidence > confidenceThreshold) {
						int idx = static_cast<int>(detectionMat.at<float>(i, 1));

						if (CLASSES[idx] == "car" | CLASSES[idx] == "bus") {
							int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
							int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
							int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
							int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

							trackBox = Rect2d((int)xLeftBottom, (int)yLeftBottom,
								(int)(xRightTop - xLeftBottom),
								(int)(yRightTop - yLeftBottom));

							bool trackBoxOk = true;
							{
								const std::lock_guard<mutex> lock(mt_trackbox);
								for (int k = 0; k < trackBoxVec.size(); k++) {

									if (k != id) {

										Rect2d trackBox_k = trackBoxVec[k];
										Point center_of_rect_k = (trackBox_k.br() + trackBox_k.tl())*0.5;
										if (center_of_rect_k.x != 0 & center_of_rect_k.y != 0) {

											Point center_of_rect_j = (trackBox.br() + trackBox.tl())*0.5;

											Point diff = center_of_rect_j - center_of_rect_k;
											float dist_Bboxes = cv::sqrt(diff.x*diff.x + diff.y*diff.y);

											// Threshold for another tracked object should be the distance between current bounding box centers
											if (dist_Bboxes < 100 | (center_of_rect_k.x == 0 & center_of_rect_k.y == 0)) {
												trackBoxOk = false;
											}
										}
									}
								}
							}

			}

		}

		else {
			this_thread::sleep_for(chrono::milliseconds(50));
		}
	}
}*/


