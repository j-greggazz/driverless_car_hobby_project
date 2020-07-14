#include <trafficDetector.h>

using namespace cv;
using namespace std;

// Implement virtual function (however not really implemented :/)
void TrafficDetector::detectObject() {

}

void TrafficDetector::setModelTxt(std::string modelText_)
{
	modelTxt = modelText_;
}

void TrafficDetector::setModelBin(std::string binText_)
{
	modelBin = binText_;
}

void TrafficDetector::detectObject(std::vector<cv::Rect2d>& trackBoxVec, std::mutex& mt_trackbox)
{

	cv::Mat frame = getCurrImg()(getRoiBox()).clone();
	Rect2d trackBox;
	Mat resizedImg;
	resize(frame, resizedImg, Size(300, 300));
	Mat inputBlob = dnn::blobFromImage(resizedImg, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);
	int id = getId();

	dnnNet.setInput(inputBlob, "data");
	Mat detection = dnnNet.forward("detection_out");
	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	ostringstream ss;
	float confidenceThreshold = .20;
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

				// Reading (Thread safe)
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

				if (trackBoxOk) {

					{
						// Writing
						const std::lock_guard<mutex> lock(mt_trackbox);
						trackBoxVec[id] = trackBox;
						td_trackbox = trackBox;
						cout << "Tracker instantiated!!: " << confidence << endl;

						ss.str("");
						ss << confidence;
						String conf(ss.str());
						String label = CLASSES[idx] + ": " + conf;
						setTrackerLabel(label);
					}
					trackingStatus = 1;
					return;
				}
			}
		}
	}
}

// Getters & Setters:

string TrafficDetector::getModelTxt()
{
	return modelTxt;
}

string TrafficDetector::getModel()
{
	return modelBin;
}

int TrafficDetector::getTrackStatus()
{
	return trackingStatus;
}

cv::Rect2d TrafficDetector::getTrackbox()
{
	return td_trackbox;
}

void TrafficDetector::setTrackerLabel(std::string trackerLabel)
{
	tracker_label = trackerLabel;
}

std::string TrafficDetector::getTrackerLabel()
{
	return tracker_label;
}

std::string* TrafficDetector::getClasses()
{
	return CLASSES;
}

void TrafficDetector::setDnnNet(cv::dnn::Net net)
{
	dnnNet = net;
}

cv::dnn::Net TrafficDetector::getDnnNet()
{
	return dnnNet;
}




