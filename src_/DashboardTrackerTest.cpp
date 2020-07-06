#include <lineDetector.h>
#include <trafficDetector.h>
#include <carTracker.h>
#include <dashboardTracker.h>
#include <calibParams.h>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <chrono>

//#define _USE_MATH_DEFINES

using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;


int initialiseVideo(VideoCapture& vCap, string path, int startFrame);

void AutoDriveTest() {

	// 1. Setup video-capture
	VideoCapture vCap;
	string path_ = "../data/dashboardVid.mp4";
	int startFrame = 10600;
	bool success = initialiseVideo(vCap, path_, startFrame);


	if (success) {
		// 2. Program flow-control variables
		char quit = 0;
		int processedFrames = 0;
		int frameCount = 0;

		// 3. Define tracker to be used:
		string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
		string trackerType = trackerTypes[2];

		// 4. Define threading-related safety variables:
		mutex lanesGuard;
		mutex trackBoxGuard;
		mutex imgAvailGuard;
		mutex trackStatusGuard;
		mutex imgProcessedGuard;
		mutex trackDescriptionGuard;
		atomic<bool> stop_threading = false;

		const int num_threads = 3;
		thread frameThreads[num_threads];

		vector<int> trackingStatus = { 0, 0, 0 }; // TODO: Need to resize/define size based on num_threads
		vector<bool> imgAvailable = { 0, 0, 0 };
		vector<bool> imgProcessed = { 0, 0, 0 };
		vector<Rect2d> trackBoxVec;
		trackBoxVec.resize(num_threads);
		vector<vector<cv::Vec4i>> lines;
		lines.resize(num_threads);
		//DashboardTracker ad = DashboardTracker();
		vector<DashboardTracker*> DashboardTrackers; 
		DashboardTrackers.resize(num_threads);

		// 5. Start Setup of program
		Mat frame;
		vCap.read(frame);
		CalibParams cb;
		CalibParams::setup(cb, frame);
	
		// 6. Create temp variables:
		vector<Rect2d> trackBoxVec_temp;
		trackBoxVec_temp.resize(num_threads);
		vector<int> trackingStatus_temp;
		trackingStatus_temp.resize(num_threads);
		vector<vector<cv::Vec4i>> lines_temp;
		lines_temp.resize(num_threads);
		vector<std::string> labels_temp;
		labels_temp.resize(num_threads);
		LineDetector ld_temp;
		TrafficDetector td_temp;

		// 7. Create member variables:

		// 7.2. Declare line-detector object
		LineDetector ld;
		ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.configParams.roi_Bbox);
		cout << ld.getRoiBox() << "accessed private variable" << endl;

		// 7.3. Declare traffic-detector object
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

		// 7.4. Declare traffic-tracker object
		CarTracker ct;
		ct.declareTracker(trackerType);

		//vector<LineDetector> LineDetectors = { ld, ld, ld };
		//vector<TrafficDetector> TrafficDetectors = { td, td, td };
		//vector<CarTracker> CarTrackers = { ct, ct, ct };
		//vector<int> Ids = { 0, 1, 2 };

		// 7.5. Declare autonomous driving objects as sum of all above objects
		// Thread_1:
		DashboardTracker *dashboardTracker_ptr0 = new DashboardTracker();
		dashboardTracker_ptr0->setLd(ld);
		dashboardTracker_ptr0->setCt(ct);
		dashboardTracker_ptr0->setTd(td);
		dashboardTracker_ptr0->setId(0);

		DashboardTracker *dashboardTracker_ptr1 = new DashboardTracker();
		dashboardTracker_ptr1->setLd(ld);
		dashboardTracker_ptr1->setCt(ct);
		dashboardTracker_ptr1->setTd(td);
		dashboardTracker_ptr1->setId(1);

		DashboardTracker *dashboardTracker_ptr2 = new DashboardTracker();
		dashboardTracker_ptr2->setLd(ld);
		dashboardTracker_ptr2->setCt(ct);
		dashboardTracker_ptr2->setTd(td);
		dashboardTracker_ptr2->setId(2);

		DashboardTrackers[0] = dashboardTracker_ptr0;
		DashboardTrackers[1] = dashboardTracker_ptr1;
		DashboardTrackers[2] = dashboardTracker_ptr2;

		// 7.6 Initialise threads: // Rewrite
		//std:thread tw = dashboardTracker_ptr->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[0] = dashboardTracker_ptr0->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[1] = dashboardTracker_ptr1->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[2] = dashboardTracker_ptr2->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));

		// 8. Start processing frames 
		
		int i = 0;
		int threadNum = 0;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);

				{
					const std::lock_guard<mutex> lock(imgAvailGuard);
					imgAvailable[threadNum] = true;
					DashboardTrackers[threadNum]->setCurrImg(frame);
				}
				threadNum++;
				frameCount++;
				if (threadNum == 3) {
					threadNum = 0;
				}

			}
			Mat curr_img;
			for (int j = 0; j < num_threads; j++) {
				bool wait = true;
				if (i % num_threads == j) {
					while (wait) {
						bool imgProcessed;
						{
							const std::lock_guard<mutex> lock(imgProcessedGuard);
							imgProcessed = DashboardTrackers[j]->getImgProcessed();
						}
						if (imgProcessed) {
							wait = false;

							// Access all tracker/detection variables from each thread and display them on current frame:
							{
								const std::lock_guard<mutex> lock(trackBoxGuard);
								ld_temp = DashboardTrackers[j]->getLd();
								trackBoxVec_temp = trackBoxVec;
								trackingStatus_temp = trackingStatus;
								lines_temp = lines;
								curr_img = DashboardTrackers[j]->getCurrImg();
								for (int k = 0; k < num_threads; ++k) {
									if (trackBoxVec_temp[k].height != 0 & trackBoxVec_temp[k].width != 0) {
										td_temp = DashboardTrackers[k]->getTd();
										string trackLabel = td_temp.getTrackerLabel();
										labels_temp[k] = trackLabel;
									}
								}
							}

							for (int k = 0; k < num_threads; ++k) {
								// Draw all trackboxes 
								Rect2d trackbox_k = trackBoxVec_temp[k];
								if (trackbox_k.height != 0 & trackbox_k.width != 0) {
									trackbox_k.x = trackbox_k.x + cb.configParams.x1_roi_car;
									trackbox_k.y = trackbox_k.y + cb.configParams.y1_roi_car;
									int status = trackingStatus_temp[k];

									if (status == 1) {
										putText(curr_img, labels_temp[k], Point(trackbox_k.x, trackbox_k.y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
										rectangle(curr_img, trackbox_k, Scalar(0, 255, 0), 1);
									}
									else if (status == 2) {
										putText(curr_img, "Tracker lost", Point(trackbox_k.x, trackbox_k.y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
										rectangle(curr_img, trackbox_k, Scalar(0, 0, 255), 1);
									}
								}
								vector<cv::Vec4i> lines_k = lines_temp[k];
								if (lines_k.size() > 0) {
									ld_temp.setLines(lines_k);
									ld_temp.drawLines(curr_img);
								}
							}
						}
						else {
							this_thread::sleep_for(milliseconds(10));
						}
					}
				}
			}
			Mat temp;
			cv::resize(curr_img, temp, cv::Size(), 0.75, 0.75);
			imshow("Frame_i", temp);
			//resizeWindow("Frame_i", 1250, int(768*1250/1366));
			{
				const std::lock_guard<mutex> lock(imgAvailGuard);
				cout << "frame " << i << endl;
			}
			processedFrames++;
			i++;
			quit = waitKey(100);
			if (quit == 'q') {
				stop_threading = true;
				for (int m = 0; m < 3; m++) {
					frameThreads[m].join();
				}
				delete dashboardTracker_ptr0;
				delete dashboardTracker_ptr1;
				delete dashboardTracker_ptr2;
				break;
			}

		}
		if (quit != 'q') {
			for (int m = 0; m < 3; m++) {
				frameThreads[m].join();
			}
			delete dashboardTracker_ptr0;
			delete dashboardTracker_ptr1;
			delete dashboardTracker_ptr2;
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


//frameThreads[i] = std::thread(DashboardTracker::dashboardTrackersThread, std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
//DashboardTracker * dashboardTracker_ptr = new DashboardTracker();
//std::thread th(&DashboardTracker::dashboardTrackersThread, dashboardTracker_ptr, std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
//frameThreads[i] = std::thread th(&DashboardTracker::dashboardTrackersThread, dashboardTracker_ptr, std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
//frameThreads[i] = dashboardTracker_ptr->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));