#include <lineDetector.h>
#include <trafficDetector.h>
#include <contourDetector.h>
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
#include <QMessageBox>


using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;


int initialiseVideo(VideoCapture& vCap, const string& path, const int& startFrame);
#if HAS_CUDA
void testGPUFunctions() {

	VideoCapture vCap;
	string video_path = "../data/dashboardVid.mp4";
	int startFrame = 10650;
	char quit = 0;
	int processedFrames = 0;
	int frameCount = 0;
	string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	string trackerType = trackerTypes[2];

	bool success = initialiseVideo(vCap, video_path, startFrame);

	if (success) {
		Mat frame;
		vCap.read(frame);
		CalibParams cb;
		CalibParams::setup(cb, frame);
		LineDetector ld;
		TrafficDetector td;
		CarTracker ct;
		cv::Ptr<cv::Tracker> m_tracker;

		ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Bbox);
		td.setRoiBox(cb.m_configParams.roi_Box_car);
		try {
			cv::dnn::Net dnnNet = dnn::readNetFromCaffe(td.getModelTxt(), td.getModel());
			// Does not work with CUDA YET!!
			//dnnNet.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
			//dnnNet.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
			td.setDnnNet(dnnNet);
			cerr << "loaded successfully" << endl;

		}
		catch (cv::Exception& e)
		{
			std::cerr << "loading failed: " << e.what() << std::endl;
			return;
		}

		ct.declareTracker(trackerType);

		int numTrackers = -1;
		while (vCap.isOpened() && quit != 113) {
			bool imgShown = false;
			bool refreshDetection = true;
			if (processedFrames == frameCount) {
				vCap.read(frame);
				ld.setCurrImg(frame);
				td.setCurrImg(frame);
				frameCount++;

				(frameCount % 5 == 0) ? (refreshDetection = true) : (refreshDetection = false);


				ld.detectObject();
				if (refreshDetection) {
					td.detectObject();
				}
				Rect2d roi_tracker_box = td.getRoiBox();
				std::vector<cv::Ptr<cv::Tracker>> trackersVec = ct.getTrackersVec();
				std::vector<cv::Rect2d> trackBoxVec = td.getTrackBoxVec();
				vector<int> erasedTrackerIndexes;

				for (int k = 0; k < trackBoxVec.size(); k++) {
					if (k <= numTrackers) {
						cv::Ptr<cv::Tracker> trackerTemp = trackersVec[k];
						bool success = trackerTemp->update(frame(roi_tracker_box), trackBoxVec[k]);
						if (!success) {
							cout << "Tracker lost!" << endl;
							numTrackers--;
							erasedTrackerIndexes.push_back(k);
						}

					}
					else {
						cv::Ptr<cv::Tracker> trackerTemp = TrackerKCF::create();
						cv::Rect2d trackbox = trackBoxVec[k];
						cv::Mat frame_tracker = frame(roi_tracker_box);

						trackerTemp->init(frame_tracker, trackbox);
						numTrackers++;
						trackersVec.push_back(trackerTemp);

					}
				}
				
				// Erased lost tracker and corresponding trackbox from respective vectors
				for (int k = 0; k < erasedTrackerIndexes.size(); k++) {
					trackersVec.erase(trackersVec.begin() + k);
					trackBoxVec.erase(trackBoxVec.begin() + k);
				}
				Mat processedImg;
				bool wait = true;
				ct.setTrackersVec(trackersVec);
				while (wait) {
					if (ld.getImgProcessed()) {
						processedImg = ld.getfinishedImg();
						wait = false;
						processedFrames++;
						for (int k = 0; k < trackBoxVec.size(); k++) {
							// Draw all trackboxes 
							Rect2d trackbox_k = trackBoxVec[k];
							if (trackbox_k.height != 0 & trackbox_k.width != 0) {
								trackbox_k.x = trackbox_k.x + cb.m_configParams.x1_roi_car;
								trackbox_k.y = trackbox_k.y + cb.m_configParams.y1_roi_car;
								rectangle(processedImg, trackbox_k, Scalar(0, 255, 0), 1);
							}
						}

					}
					else {
						this_thread::sleep_for(milliseconds(10));
					}
				}
				imshow("Image", processedImg);
				waitKey(10);
			}

		}
	}
}
#endif
void singleThreadContourTest() {

	// 1. Setup video-capture
	VideoCapture vCap;
	string video_path = "../data/dashboardVid.mp4";
	int startFrame = 10600;
	char quit = 0;
	int processedFrames = 0;
	int frameCount = 0;

	bool success = initialiseVideo(vCap, video_path, startFrame);

	if (success) {
		// 5. Start Setup of program
		Mat frame;
		vCap.read(frame);
		CalibParams cb;
		CalibParams::setup(cb, frame);
		ContourDetector cd;
		cd.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Box_car);

		cd.setCurrImg(frame);
		while (vCap.isOpened() && quit != 113) {
			bool imgShown = false;
			if (processedFrames == frameCount) {
				vCap.read(frame);
				cd.setCurrImg(frame);
				frameCount++;
				bool wait = true;
				cd.detectObject();
				while (wait) {
					if (cd.getImgProcessed()) {
						wait = false;
						processedFrames++;
					}
					else {
						this_thread::sleep_for(milliseconds(100));
					}
				}
				if (!imgShown) {
					Mat temp;
					cv::resize(cd.getShowImg(), temp, cv::Size(), 0.75, 0.75);

					imshow("Frame_i", temp);
					waitKey(100);
					imgShown = true;
				}
			}

		}
	}
}

void runThreadsOnHeap(const string& video_path) {
	// 1. Setup video-capture
	VideoCapture vCap;
	//string path_ = "../data/dashboardVid.mp4";
	int startFrame = 10600;
	bool success = initialiseVideo(vCap, video_path, startFrame);



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
		vector<TrafficDetector> trafficDetectors;
		vector<LineDetector> lineDetectors;
		vector<CarTracker> carTrackers;
		// 7. Create member variables:

		for (int k = 0; k < num_threads; k++) {
			// 7.2. Declare line-detector object
			LineDetector ld;
			ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Bbox);
			cout << ld.getRoiBox() << "accessed private variable" << endl;

			// 7.3. Declare traffic-detector object
			TrafficDetector td;
			td.setRoiBox(cb.m_configParams.roi_Box_car);

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

			lineDetectors.push_back(ld);
			trafficDetectors.push_back(td);
			carTrackers.push_back(ct);
		}
		//vector<LineDetector> LineDetectors = { ld, ld, ld };
		//vector<TrafficDetector> TrafficDetectors = { td, td, td };
		//vector<CarTracker> CarTrackers = { ct, ct, ct };
		//vector<int> Ids = { 0, 1, 2 };

		// 7.5. Declare autonomous driving objects as sum of all above objects
		// Thread_1:
		DashboardTracker* dashboardTracker_ptr0 = new DashboardTracker();
		dashboardTracker_ptr0->setLd(lineDetectors[0]);
		dashboardTracker_ptr0->setCt(carTrackers[0]);
		dashboardTracker_ptr0->setTd(trafficDetectors[0]);
		dashboardTracker_ptr0->setId(0);

		DashboardTracker* dashboardTracker_ptr1 = new DashboardTracker();
		dashboardTracker_ptr1->setLd(lineDetectors[1]);
		dashboardTracker_ptr1->setCt(carTrackers[1]);
		dashboardTracker_ptr1->setTd(trafficDetectors[1]);
		dashboardTracker_ptr1->setId(1);

		DashboardTracker* dashboardTracker_ptr2 = new DashboardTracker();
		dashboardTracker_ptr2->setLd(lineDetectors[2]);
		dashboardTracker_ptr2->setCt(carTrackers[2]);
		dashboardTracker_ptr2->setTd(trafficDetectors[2]);
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
		auto start = std::chrono::high_resolution_clock::now();
		int i = 0;
		int threadNum = 0;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);

				{	// Writing
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

						// Reading (Thread-safe)
						imgProcessed = DashboardTrackers[j]->getImgProcessed();

						if (imgProcessed) {
							wait = false;
							start = std::chrono::high_resolution_clock::now();

							// Access all tracker/detection variables from each thread and display them on current frame:

							// Reading (Thread-safe)
							ld_temp = DashboardTrackers[j]->getLd();
							trackBoxVec_temp = trackBoxVec;
							trackingStatus_temp = trackingStatus;
							lines_temp = lines;
							curr_img = DashboardTrackers[j]->getCurrImg();


							for (int k = 0; k < num_threads; ++k) {
								if ((trackBoxVec_temp[k].height != 0) & (trackBoxVec_temp[k].width != 0)) {

									// Reading (Thread-safe)
									td_temp = DashboardTrackers[k]->getTd();
									string trackLabel = td_temp.getTrackerLabel();
									labels_temp[k] = trackLabel;
								}

							}

							for (int k = 0; k < num_threads; ++k) {
								// Draw all trackboxes 
								Rect2d trackbox_k = trackBoxVec_temp[k];
								if ((trackbox_k.height != 0) & (trackbox_k.width != 0)) {
									trackbox_k.x = trackbox_k.x + cb.m_configParams.x1_roi_car;
									trackbox_k.y = trackbox_k.y + cb.m_configParams.y1_roi_car;
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
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - start;

			{
				// Writing to 'cout'
				const std::lock_guard<mutex> lock(imgAvailGuard);
				cout << "frame " << i << endl;
				std::cout << "Displaying latest status took : " << elapsed.count() << " s\n";
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

	else {
		cout << "invalid file provided. " << endl;
	}
	return;
}

void runThreadsOnStack(const string& video_path) {

	// 1. Setup video-capture
	VideoCapture vCap;
	string path_ = "../data/dashboardVid.mp4";
	int startFrame = 10600;
	bool success = initialiseVideo(vCap, video_path, startFrame);


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
		vector<DashboardTracker> DashboardTrackers;
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
		vector<TrafficDetector> trafficDetectors;
		vector<LineDetector> lineDetectors;
		vector<CarTracker> carTrackers;
		// 7. Create member variables:

		for (int k = 0; k < num_threads; k++) {
			// 7.2. Declare line-detector object
			LineDetector ld;
			ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Bbox);
			cout << ld.getRoiBox() << "accessed private variable" << endl;

			// 7.3. Declare traffic-detector object
			TrafficDetector td;
			td.setRoiBox(cb.m_configParams.roi_Box_car);

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

			lineDetectors.push_back(ld);
			trafficDetectors.push_back(td);
			carTrackers.push_back(ct);
		}
		//vector<LineDetector> LineDetectors = { ld, ld, ld };
		//vector<TrafficDetector> TrafficDetectors = { td, td, td };
		//vector<CarTracker> CarTrackers = { ct, ct, ct };
		//vector<int> Ids = { 0, 1, 2 };

		// 7.5. Declare autonomous driving objects as sum of all above objects
		// Thread_1:
		//DashboardTracker *dashboardTracker_ptr0 = new DashboardTracker();
		DashboardTracker dashboardTracker0;
		dashboardTracker0.setLd(lineDetectors[0]);
		dashboardTracker0.setCt(carTrackers[0]);
		dashboardTracker0.setTd(trafficDetectors[0]);
		dashboardTracker0.setId(0);

		DashboardTracker dashboardTracker1;
		dashboardTracker1.setLd(lineDetectors[1]);
		dashboardTracker1.setCt(carTrackers[1]);
		dashboardTracker1.setTd(trafficDetectors[1]);
		dashboardTracker1.setId(1);

		DashboardTracker dashboardTracker2;
		dashboardTracker2.setLd(lineDetectors[2]);
		dashboardTracker2.setCt(carTrackers[2]);
		dashboardTracker2.setTd(trafficDetectors[2]);
		dashboardTracker2.setId(2);

		DashboardTrackers[0] = dashboardTracker0;
		DashboardTrackers[1] = dashboardTracker1;
		DashboardTrackers[2] = dashboardTracker2;

		// 7.6 Initialise threads: // Rewrite
		//std:thread tw = dashboardTracker_ptr->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[0] = DashboardTrackers[0].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[1] = DashboardTrackers[1].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[2] = DashboardTrackers[2].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));

		// 8. Start processing frames 
		auto start = std::chrono::high_resolution_clock::now();
		int i = 0;
		int threadNum = 0;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);

				{	// Writing
					const std::lock_guard<mutex> lock(imgAvailGuard);
					imgAvailable[threadNum] = true;
					DashboardTrackers[threadNum].setCurrImg(frame);
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

						// Reading (Thread-safe)
						imgProcessed = DashboardTrackers[j].getImgProcessed();

						if (imgProcessed) {
							wait = false;
							start = std::chrono::high_resolution_clock::now();

							// Access all tracker/detection variables from each thread and display them on current frame:

							// Reading (Thread-safe)
							ld_temp = DashboardTrackers[j].getLd();
							trackBoxVec_temp = trackBoxVec;
							trackingStatus_temp = trackingStatus;
							lines_temp = lines;
							curr_img = DashboardTrackers[j].getCurrImg();


							for (int k = 0; k < num_threads; ++k) {
								if (trackBoxVec_temp[k].height != 0 & trackBoxVec_temp[k].width != 0) {

									// Reading (Thread-safe)
									td_temp = DashboardTrackers[k].getTd();
									string trackLabel = td_temp.getTrackerLabel();
									labels_temp[k] = trackLabel;
								}

							}

							for (int k = 0; k < num_threads; ++k) {
								// Draw all trackboxes 
								Rect2d trackbox_k = trackBoxVec_temp[k];
								if (trackbox_k.height != 0 & trackbox_k.width != 0) {
									trackbox_k.x = trackbox_k.x + cb.m_configParams.x1_roi_car;
									trackbox_k.y = trackbox_k.y + cb.m_configParams.y1_roi_car;
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
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - start;

			{
				// Writing to 'cout'
				const std::lock_guard<mutex> lock(imgAvailGuard);
				cout << "frame " << i << endl;
				std::cout << "Displaying latest status took : " << elapsed.count() << " s\n";
			}
			processedFrames++;
			i++;
			quit = waitKey(100);
			if (quit == 'q') {
				stop_threading = true;
				for (int m = 0; m < 3; m++) {
					frameThreads[m].join();
				}

				break;
			}

		}
		if (quit != 'q') {
			for (int m = 0; m < 3; m++) {
				frameThreads[m].join();
			}
		}

		return;
	}

}

void runStaticMethodThreads(const string& video_path, const string& cur_dir) {

	// 0. Determine working directory for reference to loaded models, data
	int pos = cur_dir.find_last_of("/\\");
	std::string filename = cur_dir.substr(pos + 1);
	std::string slash = "\\";
	slash.append(filename);
	std::string folder = cur_dir.substr(0, cur_dir.size() - slash.size());
	pos = folder.find_last_of("/\\");
	std::cout << "folder: " << folder.substr(pos + 1) << std::endl;
	folder = folder.substr(pos + 1);

	// 1. Setup video-capture
	VideoCapture vCap;
	string path_ = "../data/dashboardVid.mp4";
	int startFrame = 11250;// 10750;
	bool success = initialiseVideo(vCap, video_path, startFrame);

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
		
		const int num_threads = 2;
		thread frameThreads[num_threads];

		vector<int> trackingStatus(num_threads, 0); // TODO: Need to resize/define size based on num_threads
		vector<bool> imgAvailable(num_threads, 0);
		vector<bool> imgProcessed(num_threads, 0);
		vector<Rect2d> trackBoxVec;
		trackBoxVec.resize(num_threads);
		vector<vector<cv::Vec4i>> lines;
		lines.resize(num_threads);
		DashboardTracker dt;// = DashboardTracker();
		vector<DashboardTracker> DashboardTrackers(num_threads, dt);

		// 5. Start Setup of program
		Mat frame;
		vCap.read(frame);
		CalibParams cb;
		CalibParams::setup(cb, frame);

		// 6. Create temp variables:
		vector<Rect2d> trackBoxVec_temp;
		vector<int> trackingStatus_temp;
		vector<vector<cv::Vec4i>> lines_temp;
		vector<std::string> labels_temp;
		labels_temp.resize(num_threads);
		LineDetector ld_temp;
		TrafficDetector td_temp;


		// 7. Initialise threads
		for (int i = 0; i < num_threads; i++) {

			// 7.1. Declare id for given objects
			int id = i;

			// 7.2. Declare line-detector object
			LineDetector ld;
			ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Bbox);
			//cout << ld.getRoiBox() << "accessed private variable" << endl;

			// 7.3. Declare traffic-detector object
			TrafficDetector td;
			td.setRoiBox(cb.m_configParams.roi_Box_car);

			if (folder == "Release") {
				td.setModelTxt("../../models/MobileNetSSD_deploy.prototxt.txt");
				td.setModelBin("../../models/MobileNetSSD_deploy.caffemodel");
			}

			try {
				dnn::Net net = dnn::readNetFromCaffe(td.getModelTxt(), td.getModel());
				net.setPreferableBackend(DNN_BACKEND_CUDA);
				net.setPreferableTarget(DNN_TARGET_CUDA);
				td.setDnnNet(net);
				
				//dnn::Net net = cv::dnn::readNetFromTensorflow("../models/frozen_inference_graph.pb", "../models/labelmap.pbtxt");
				//bool empty = net.empty();
				//dnn::Net net = net = dnn::readNetFromCaffe(td.getModelTxt(), td.getModel());
				//empty = net.empty();
				//td.setDnnNet(net);
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

			// 7.5. Declare autonomous driving object as sum of all above objects
			DashboardTrackers[i].setLd(ld);
			DashboardTrackers[i].setCt(ct);
			DashboardTrackers[i].setTd(td);
			DashboardTrackers[i].setId(i);

			// 7.6 Initialise threads: // Rewrite
			frameThreads[i] = std::thread(DashboardTracker::staticMethodThread, std::ref(DashboardTrackers[i]), std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		}

		// 8. Start processing frames 

		int i = 0;
		int threadNum = 0;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);

				{	// Writing
					const std::lock_guard<mutex> lock(imgAvailGuard);
					imgAvailable[threadNum] = true;
					DashboardTrackers[threadNum].setCurrImg(frame);
				}

				threadNum++;
				frameCount++;
				if (threadNum == num_threads) {
					threadNum = 0;
				}

			}

			Mat curr_img;
			for (int j = 0; j < num_threads; j++) {
				bool wait = true;
				if (i % num_threads == j) {

					while (wait) {
						bool imgProcessed;

						//  Reading
						imgProcessed = DashboardTrackers[j].getImgProcessed();

						if (imgProcessed) {
							wait = false;

							// Access all tracker/detection variables from each thread and display them on current frame:

							// Reading (thread safe)
							ld_temp = DashboardTrackers[j].getLd();
							trackBoxVec_temp = trackBoxVec;
							trackingStatus_temp = trackingStatus;
							lines_temp = lines;
							curr_img = DashboardTrackers[j].getCurrImg();

							for (int k = 0; k < num_threads; ++k) {
								if (trackBoxVec_temp[k].height != 0 & trackBoxVec_temp[k].width != 0) {
									td_temp = DashboardTrackers[k].getTd();
									string trackLabel = td_temp.getTrackerLabel();
									labels_temp[k] = trackLabel;
								}
							}

							for (int k = 0; k < num_threads; ++k) {
								// Draw all trackboxes 
								Rect2d trackbox_k = trackBoxVec_temp[k];
								if (trackbox_k.height != 0 & trackbox_k.width != 0) {
									trackbox_k.x = trackbox_k.x + cb.m_configParams.x1_roi_car;
									trackbox_k.y = trackbox_k.y + cb.m_configParams.y1_roi_car;
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

			//{
			//	const std::lock_guard<mutex> lock(imgAvailGuard);
			//	cout << "frame " << i << endl;
			//}
			processedFrames++;
			i++;
			quit = waitKey(100);
			if (quit == 'q') {
				stop_threading = true;
				for (int m = 0; m < num_threads; m++) {
					frameThreads[m].join();
				}
				break;
			}


		}
		return;
	}

}

int initialiseVideo(VideoCapture& vCap, const string& path, const int& startFrame) {

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

void runThreadsOnStack_GPU(const string& video_path) {

	// 1. Setup video-capture
	VideoCapture vCap;
	int startFrame = 10600;
	bool success = initialiseVideo(vCap, video_path, startFrame);

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
		vector<DashboardTracker> DashboardTrackers;
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
		vector<TrafficDetector> trafficDetectors;
		vector<LineDetector> lineDetectors;
		vector<CarTracker> carTrackers;
		// 7. Create member variables:

		for (int k = 0; k < num_threads; k++) {
			// 7.2. Declare line-detector object
			LineDetector ld;
			ld.setParams(cb.getPreprocessParams(), cb.getHoughParams(), cb.m_configParams.roi_Bbox);

			// 7.3. Declare traffic-detector object
			TrafficDetector td;
			td.setRoiBox(cb.m_configParams.roi_Box_car);

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

			lineDetectors.push_back(ld);
			trafficDetectors.push_back(td);
			carTrackers.push_back(ct);
		}
		//vector<LineDetector> LineDetectors = { ld, ld, ld };
		//vector<TrafficDetector> TrafficDetectors = { td, td, td };
		//vector<CarTracker> CarTrackers = { ct, ct, ct };
		//vector<int> Ids = { 0, 1, 2 };

		// 7.5. Declare autonomous driving objects as sum of all above objects
		// Thread_1:
		//DashboardTracker *dashboardTracker_ptr0 = new DashboardTracker();
		DashboardTracker dashboardTracker0;
		dashboardTracker0.setLd(lineDetectors[0]);
		dashboardTracker0.setCt(carTrackers[0]);
		dashboardTracker0.setTd(trafficDetectors[0]);
		dashboardTracker0.setId(0);

		DashboardTracker dashboardTracker1;
		dashboardTracker1.setLd(lineDetectors[1]);
		dashboardTracker1.setCt(carTrackers[1]);
		dashboardTracker1.setTd(trafficDetectors[1]);
		dashboardTracker1.setId(1);

		DashboardTracker dashboardTracker2;
		dashboardTracker2.setLd(lineDetectors[2]);
		dashboardTracker2.setCt(carTrackers[2]);
		dashboardTracker2.setTd(trafficDetectors[2]);
		dashboardTracker2.setId(2);

		DashboardTrackers[0] = dashboardTracker0;
		DashboardTrackers[1] = dashboardTracker1;
		DashboardTrackers[2] = dashboardTracker2;

		// 7.6 Initialise threads: // Rewrite
		//std:thread tw = dashboardTracker_ptr->dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[0] = DashboardTrackers[0].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[1] = DashboardTrackers[1].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[2] = DashboardTrackers[2].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));

		// 8. Start processing frames 
		auto start = std::chrono::high_resolution_clock::now();
		int i = 0;
		int threadNum = 0;
		while (vCap.isOpened() && quit != 113) {
			if (processedFrames == frameCount) {
				vCap.read(frame);

				{	// Writing
					const std::lock_guard<mutex> lock(imgAvailGuard);
					imgAvailable[threadNum] = true;
					DashboardTrackers[threadNum].setCurrImg(frame);
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

						// Reading (Thread-safe)
						imgProcessed = DashboardTrackers[j].getImgProcessed();

						if (imgProcessed) {
							wait = false;
							start = std::chrono::high_resolution_clock::now();

							// Access all tracker/detection variables from each thread and display them on current frame:

							// Reading (Thread-safe)
							ld_temp = DashboardTrackers[j].getLd();
							trackBoxVec_temp = trackBoxVec;
							trackingStatus_temp = trackingStatus;
							lines_temp = lines;
							curr_img = DashboardTrackers[j].getCurrImg();


							for (int k = 0; k < num_threads; ++k) {
								if (trackBoxVec_temp[k].height != 0 & trackBoxVec_temp[k].width != 0) {

									// Reading (Thread-safe)
									td_temp = DashboardTrackers[k].getTd();
									string trackLabel = td_temp.getTrackerLabel();
									labels_temp[k] = trackLabel;
								}

							}

							for (int k = 0; k < num_threads; ++k) {
								// Draw all trackboxes 
								Rect2d trackbox_k = trackBoxVec_temp[k];
								if (trackbox_k.height != 0 & trackbox_k.width != 0) {
									trackbox_k.x = trackbox_k.x + cb.m_configParams.x1_roi_car;
									trackbox_k.y = trackbox_k.y + cb.m_configParams.y1_roi_car;
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
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - start;

			{
				// Writing to 'cout'
				const std::lock_guard<mutex> lock(imgAvailGuard);
				cout << "frame " << i << endl;
				std::cout << "Displaying latest status took : " << elapsed.count() << " s\n";
			}
			processedFrames++;
			i++;
			quit = waitKey(100);
			if (quit == 'q') {
				stop_threading = true;
				for (int m = 0; m < 3; m++) {
					frameThreads[m].join();
				}

				break;
			}

		}
		if (quit != 'q') {
			for (int m = 0; m < 3; m++) {
				frameThreads[m].join();
			}
		}

		return;
	}

}


