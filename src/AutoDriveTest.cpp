#include <lineDetector.h>
#include <trafficDetector.h>
#include <carTracker.h>
#include <autoDrive.h>
#include <calibParams.h>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <chrono>
//#include <thread>

//#define _USE_MATH_DEFINES

using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;


int initialiseVideo(VideoCapture& vCap, string path, int startFrame);

void AutoDriveTest() {
	//AutoDrive ad(LineDetector ld, TrafficDetector td, CarTracker ct);

	// 1. Setup video-capture
	VideoCapture vCap;
	string path_ = "../data/dashboardVid.mp4";
	int startFrame = 10500;
	bool success = initialiseVideo(vCap, path_, startFrame);
	

	if (success) {
		// 2. Program flow-control variables
		char quit = 0;
		int processedFrames = 0;
		int frameCount = 0;

		// 3. Define tracker to be used:
		string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
		string trackerType = trackerTypes[2];

		// 4. Define threading-related variables:
		const int num_threads = 3;
		thread frameThreads[num_threads];
		std::atomic<bool> stop_threading = false;
		vector<Rect2d> trackBoxVec;
		trackBoxVec.resize(num_threads);
		mutex trackBoxReserve;
		vector<vector<cv::Vec4i>> lines;
		lines.resize(num_threads);
		mutex laneLinesReserve;
		vector<atomic<int>> trackingStatus = { 0, 0, 0 };
		vector<atomic<bool>> imgAvailable = { 0, 0, 0 };
		vector<AutoDrive> AutoDrives;

		// 5. Start Setup of program
		Mat frame;
		vCap.read(frame);
		CalibParams cb;
		CalibParams::setup(cb, frame);


		// 6. Initialise threads
		for (int i = 0; i < num_threads; i++) {

			// 6.1. Declare id for given objects
			int id = i;

			// 6.2. Declare line-detector object
			LineDetector ld;
			ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.configParams.roi_Bbox);
			cout << ld.getRecWidth() << "accessed private variable" << endl;

			// 6.3. Declare traffic-detector object
			TrafficDetector td;
			td.setRoiBox(cb.configParams.roi_Box_car);

			try {
				td.setDnnNet(dnn::readNetFromCaffe(td.getModelTxt(), td.getModel()));
				cerr << "loaded successfully" << endl;
			}
			catch (cv::Exception& e)
			{
				std::cerr << "loading failed: " << e.what() << std::endl;
				return;
			}

			// 6.4. Declare traffic-tracker object
			CarTracker ct;
			ct.declareTracker(trackerType);

			// 6.5. Declare autonomous driving object as sum of all above objects
			AutoDrive ad = AutoDrive(id, ld, td, ct);
			AutoDrives.push_back(ad);

			// 6.6 Initialise threads:
			std::thread(AutoDrive::autoDriveThread, std::ref(ad), std::ref(imgAvailable[i]), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus[i]), std::ref(lines), std::ref(trackBoxReserve), std::ref(laneLinesReserve));
		}

		// 7. Initialise thread and thread variables:
			   			

			

		int i = -1;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);
				frameCount++;
				i++;
			}

			for (int j = 0; j < num_threads; j++) {
				if (i % num_threads == j) {


				}


			}


			//else {
			//	this_thread::sleep_for(microseconds(10));
			//}













		}
		return;
	}

}





int initialiseVideo(VideoCapture& vCap, string path, int startFrame) {

	vCap.open(path);

	if (!vCap.isOpened()) {
		cout << "Error reading file: Check filepath." << endl;
		waitKey();
		return 0;
	}

	else if (vCap.get(CAP_PROP_FRAME_COUNT) < 1) {
		cout << "Error: At least one frame is required" << endl;
		waitKey();
		return(0);
	}

	bool success = vCap.set(CAP_PROP_POS_FRAMES, startFrame);
	return success;
}
