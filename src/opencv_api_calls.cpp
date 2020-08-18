#include "opencv_api_calls.h"
#include "lineDetector.h"
#include "calibParams.h"

using namespace cv;
using namespace std;

bool OPENCV_API_CALLS::check_VideoCaptureWorks()
{
	string video_path = "../../data/dashboardVid.mp4";
	int startFrame = 10650;
	VideoCapture vCap;
	vCap.open(video_path);
	bool success = 0;

	if (!vCap.isOpened()) {
		cout << "Error reading file: Check filepath." << endl;
	}
	else {
		success = vCap.set(CAP_PROP_POS_FRAMES, startFrame);
	}

	return success;
}

bool OPENCV_API_CALLS::check_ObjectTrackerInitialisation(int tracker_index)
{
	string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	string trackerType = trackerTypes[tracker_index];
	cv::Ptr<cv::Tracker> tracker;

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

	return tracker.empty() ? false : true;
}

bool OPENCV_API_CALLS::check_ObjectDetectorInitialisation()
{
	std::string modelTxt = "../../models/MobileNetSSD_deploy.prototxt.txt";
	std::string modelBin = "../../models/MobileNetSSD_deploy.caffemodel";

	try {
		cv::dnn::Net dnnNet = dnn::readNetFromCaffe(modelTxt, modelBin);
		cerr << "loaded successfully" << endl;
		return true;
	}
	catch (cv::Exception& e)
	{
		std::cerr << "loading failed: " << e.what() << std::endl;
		return false;
	}

	return false;
}

bool OPENCV_API_CALLS::detectAndDrawLines()
{
	Mat test_img = cv::imread("../../data/testLinesImg2.png", IMREAD_COLOR);
	int x_offset = 400;  //390
	int y_offset = 850;  //622
	int recWidth = 1000;  //1400
	int recHeight = 200;
	cv::Rect roi_Bbox = cv::Rect(x_offset, y_offset, recWidth, recHeight);
	test_img = test_img(roi_Bbox);


#if HAS_CUDA

	cv::cuda::GpuMat gpuImg, grayImg, mask;
	gpuImg.upload(test_img);
	cv::cuda::cvtColor(gpuImg, grayImg, COLOR_BGR2GRAY);

	LineDetector ld;
	CalibParams cb;


	cv::Ptr<cv::cuda::HoughSegmentDetector> houghDetector = cv::cuda::createHoughSegmentDetector(1, CV_PI / 180, cb.m_configParams.minLineLength, cb.m_configParams.maxLineGap);
	cv::Ptr<cv::cuda::CannyEdgeDetector> m_CannyDetector = cv::cuda::createCannyEdgeDetector(cb.m_configParams.lowThresh, cb.m_configParams.highThresh, cb.m_configParams.cannyKernel + 3);
	cv::Ptr<cv::cuda::Filter> m_BoxFilter = cv::cuda::createBoxFilter(CV_8U, CV_8U, cv::Size(cb.m_configParams.gauss_ksize, cb.m_configParams.gauss_ksize), cv::Point(-1, -1));

	// 2. Blur
	if (cb.m_configParams.gauss_ksize > 0) {
		m_BoxFilter->apply(grayImg, grayImg);
	}

	if (!grayImg.empty()) {
		m_CannyDetector->detect(grayImg, mask);
	}

	cv::cuda::GpuMat d_lines2;
	std::vector<cv::Vec4i> m_lines;
	houghDetector->detect(mask, d_lines2);
	

	if (!d_lines2.empty()) {
		m_lines.resize(d_lines2.cols);
		Mat h_lines(1, d_lines2.cols, CV_32SC4, &m_lines[0]);
		d_lines2.download(h_lines);
	}

	//ld.setLines(m_lines);
	//ld.drawLines(test_img, false, true);
	

#endif
	//imshow("Example_test", test_img);
	//waitKey();

	return (m_lines.size() > 0) ? true : false;
}






