#include <trafficDetector.h>

using namespace cv;
using namespace std;

// Implement virtual function (however not really implemented :/)
void TrafficDetector::detectObject() {

// C++ Cuda not able to do object tracking yet; See -> https://github.com/opencv/opencv/issues/16433 ("GpuMat as input/output to cv::dnn::Net")
#if HAS_CUDA
	cv::Mat frame = getCurrImg()(getRoiBox()).clone();
	Rect2d trackBox;
	Mat resizedImg;
	cv::resize(frame, resizedImg, Size(300, 300));
	
	Mat inputBlob = dnn::blobFromImage(resizedImg, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);
	//cv::cuda::GpuMat gpuBlob, mask;
	//gpuBlob.upload(inputBlob);
	int id = getId();
	cv::Mat detection; 
	//m_dnnNet.setInput(gpuBlob, "data");
	m_dnnNet.setInput(inputBlob, "data");
	detection = m_dnnNet.forward("detection_out");
	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	ostringstream ss;
	float confidenceThreshold = .20;
	float dist_Bboxes;
	for (int i = 0; i < detectionMat.rows; i++) {
		float confidence = detectionMat.at<float>(i, 2);

		if (confidence > confidenceThreshold) {
			int idx = static_cast<int>(detectionMat.at<float>(i, 1));

			if (m_CLASSES[idx] == "car" | m_CLASSES[idx] == "bus" ) {
				int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
				int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
				int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
				int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

				trackBox = Rect2d((int)xLeftBottom, (int)yLeftBottom, (int)(xRightTop - xLeftBottom), (int)(yRightTop - yLeftBottom));
				bool trackBoxOk = true;

				// Reading (Thread safe)
				for (int k = 0; k < m_trackBoxVec.size(); k++) {

					Rect2d trackBox_k = m_trackBoxVec[k];
					Point center_of_rect_k = (trackBox_k.br() + trackBox_k.tl()) * 0.5;
					if ((center_of_rect_k.x != 0) & (center_of_rect_k.y != 0)) {

						Point center_of_rect_j = (trackBox.br() + trackBox.tl()) * 0.5;

						Point diff = center_of_rect_j - center_of_rect_k;
						dist_Bboxes = cv::sqrt(diff.x * diff.x + diff.y * diff.y);

						// Threshold for another tracked object should be the distance between current bounding box centers
						if ((dist_Bboxes < 300) | ((center_of_rect_k.x == 0) & (center_of_rect_k.y == 0))) {
							trackBoxOk = false;
						}

					}
				}

				if (trackBoxOk) {

					m_trackBoxVec.push_back(trackBox);
					cout << "Tracker instantiated!!: " << confidence << endl;

					ss.str("");
					ss << confidence;
					String conf(ss.str());
					String label = m_CLASSES[idx] + ": " + conf;
					setTrackerLabel(label);

					return;
				}
			}
		}
	}
#endif
}

void TrafficDetector::setModelTxt(const std::string& modelText_)
{
	m_modelTxt = modelText_;
}

void TrafficDetector::setModelBin(const std::string& binText_)
{
	m_modelBin = binText_;
}


void TrafficDetector::detectObject(std::vector<cv::Rect2d>& trackBoxVec, std::mutex& mt_trackbox)
{
	
	cv::Mat frame = getCurrImg()(getRoiBox()).clone();
	Rect2d trackBox;
	Mat resizedImg;
	cv::resize(frame, resizedImg, Size(300, 300));
	Mat inputBlob = dnn::blobFromImage(resizedImg, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);

	int id = getId();
	//cout << m_dnnNet.empty() << endl;
	m_dnnNet.setInput(inputBlob, "data");
	Mat detection = m_dnnNet.forward("detection_out");
	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	ostringstream ss;
	float confidenceThreshold = .20;
	for (int i = 0; i < detectionMat.rows; i++) {
		float confidence = detectionMat.at<float>(i, 2);

		if (confidence > confidenceThreshold) {
			int idx = static_cast<int>(detectionMat.at<float>(i, 1));

			if (m_CLASSES[idx] == "car" | m_CLASSES[idx] == "bus" | m_CLASSES[idx] == "bicycle" | m_CLASSES[idx] == "person") {
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
						Point center_of_rect_k = (trackBox_k.br() + trackBox_k.tl()) * 0.5;
						if ((center_of_rect_k.x != 0) & (center_of_rect_k.y != 0)) {

							Point center_of_rect_j = (trackBox.br() + trackBox.tl()) * 0.5;

							Point diff = center_of_rect_j - center_of_rect_k;
							float dist_Bboxes = cv::sqrt(diff.x * diff.x + diff.y * diff.y);

							// Threshold for another tracked object should be the distance between current bounding box centers
							if ((dist_Bboxes < 100) | ((center_of_rect_k.x == 0) & (center_of_rect_k.y == 0))) {
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
						m_trackbox = trackBox;
						cout << "Tracker instantiated!!: " << confidence << endl;

						ss.str("");
						ss << confidence;
						String conf(ss.str());
						String label = m_CLASSES[idx] + ": " + conf;
						setTrackerLabel(label);
					}
					m_trackingStatus = 1;
					return;
				}
			}
		}
	}
}

// Getters & Setters:

string TrafficDetector::getModelTxt() const
{
	return m_modelTxt;
}

string TrafficDetector::getModel() const
{
	return m_modelBin;
}

int TrafficDetector::getTrackStatus() const
{
	return m_trackingStatus;
}

cv::Rect2d TrafficDetector::getTrackbox() const
{
	return m_trackbox;
}

void TrafficDetector::setTrackerLabel(const std::string& trackerLabel)
{
	m_trackerLabel = trackerLabel;
}

std::string TrafficDetector::getTrackerLabel() const
{
	return m_trackerLabel;
}

std::vector<cv::Rect2d> TrafficDetector::getTrackBoxVec() const
{
	return m_trackBoxVec;
}

std::string* TrafficDetector::getClasses() 
{
	return m_CLASSES;
}

void TrafficDetector::setDnnNet(const cv::dnn::Net& net)
{
	//cout << net.empty() << endl;
	m_dnnNet = net;
}

cv::dnn::Net TrafficDetector::getDnnNet() const
{
	return m_dnnNet;
}




