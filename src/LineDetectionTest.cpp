#include <lineDetector.h>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <atomic> 
#include <string>
//#define _USE_MATH_DEFINES

using namespace cv;
using namespace std;

void setup(LineDetector& ld_, Mat& img);
int loadVideo(bool multithreading = true);
void lineDetectionTest() {

	loadVideo();
}

int loadVideo(bool multithreading) {

	LineDetector ld;

	if (multithreading) {

		//--- INITIALIZE VIDEOCAPTURE
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");
		bool firstFrame = false;

		// atomic variable to stop threads
		std::atomic<bool> stop_threading = false;

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

		else {
			//cout << "Number of frames = " << vCap.get(CAP_PROP_FRAME_COUNT) << endl;
			vCap.read(frame);
			char quit = 0; 

			int i = 0;
			const int num_threads = 3;

			vector<LineDetector> lineDetectors; 
			lineDetectors.resize(num_threads);
			thread frameThreads[num_threads];

			while (vCap.isOpened() && quit != 113) {

				if (quit == 'q') {
					for (int i = 0; i < 3; i++) {
						frameThreads[i].join();
					}
					break;
				}
				if (firstFrame) {
					setup(ld, frame);
					firstFrame = false;
				}

				else if (i < 3) {
					vCap.read(frame);
					lineDetectors[i].configParams.edgeParams.id = i;
					lineDetectors[i].configParams.edgeParams.currImg = frame;
					lineDetectors[i].configParams.edgeParams.newImgAvailable = true;
					lineDetectors[i].configParams.edgeParams.continueProcessing = true;
					frameThreads[i] = std::thread(LineDetector::processImg_thread, std::ref(lineDetectors[i]), std::ref(stop_threading));
					i++;
				}


				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					vCap.read(frame);

					if (i % num_threads == 0) {
						try {
							if (lineDetectors[0].configParams.edgeParams.linesDrawn == true) {
								imshow("Sample", lineDetectors[0].configParams.edgeParams.currImg.clone());
								lineDetectors[0].configParams.edgeParams.linesDrawn == false;
								//cout << "%3 == 0, frame " << i << endl;
								lineDetectors[0].configParams.edgeParams.currImg = frame;
								lineDetectors[0].configParams.edgeParams.newImgAvailable = true;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
					}


					if (i % num_threads == 1) {
						try {
							if (lineDetectors[1].configParams.edgeParams.linesDrawn == true) {
								imshow("Sample", lineDetectors[1].configParams.edgeParams.currImg.clone());
								lineDetectors[1].configParams.edgeParams.linesDrawn == false;
								//cout << "%3 == 1, frame " << i << endl;
								lineDetectors[1].configParams.edgeParams.currImg = frame;
								lineDetectors[1].configParams.edgeParams.newImgAvailable = true;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
					
					}

					if (i % num_threads == 2) {
						try {
							if (lineDetectors[2].configParams.edgeParams.linesDrawn == true) {
								imshow("Sample", lineDetectors[2].configParams.edgeParams.currImg.clone());
								lineDetectors[2].configParams.edgeParams.linesDrawn == false;
								//cout << "%3 == 2, frame " << i << endl;
								lineDetectors[2].configParams.edgeParams.currImg = frame;
								lineDetectors[2].configParams.edgeParams.newImgAvailable = true;
							}
						}
						catch (cv::Exception& e) {
							int val = 0;
						}					
					}
					i++;
					quit = waitKey(10);
				}
			}
			if (quit == 'q') {
				stop_threading = true;
				for (int i = 0; i < 3; i++) {
					frameThreads[i].join();
				}
			}
			destroyWindow("Sample");
			return 1;
		}
	}

	else {
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");
		bool firstFrame = false;

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

		else {
			//cout << "Number of frames = " << vCap.get(CAP_PROP_FRAME_COUNT) << endl;
			vCap.read(frame);
			char quit = 0; // Ascii value is 113

			int i = 0;
			while (vCap.isOpened() && quit != 113) {

				if (quit == 'q') {
					quit = true;
					break;
				}
				if (firstFrame) {
					setup(ld, frame);
					firstFrame = false;
				}

				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					vCap.read(frame);

					ld.configParams.edgeParams.currImg = frame;

					ld.processImg();

					imshow("Sample", frame);

					quit = waitKey(10);

					/*auto start = std::chrono::high_resolution_clock::now();
					vCap.read(frame);
					auto finish = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = finish - start;
					cout << "Time taken to read frame = " << elapsed.count() << endl;

					ld.configParams.edgeParams.currImg = frame;
					start = std::chrono::high_resolution_clock::now();
					ld.processImg();
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for image processing = " << elapsed.count() << endl;
					start = std::chrono::high_resolution_clock::now();
					imshow("Sample", frame);
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for imshow = " << elapsed.count() << endl;
					waitKey(100);*/
				}


			}
			destroyWindow("Sample");
			return 1;
		}
	}

}

int loadVideo_(bool multithreading) {

	LineDetector ld;
	if (!multithreading) {
		//--- INITIALIZE VIDEOCAPTURE
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");
		bool firstFrame = true;

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

		else {
			//cout << "Number of frames = " << vCap.get(CAP_PROP_FRAME_COUNT) << endl;
			vCap.read(frame);
			char quit = 0; // Ascii value is 113

			int i = 0;
			while (vCap.isOpened() && quit != 113) {

				if (quit == 'q') {
					quit = true;
					break;
				}
				if (firstFrame) {
					setup(ld, frame);
					firstFrame = false;
				}

				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					vCap.read(frame);


					ld.configParams.edgeParams.currImg = frame;

					ld.processImg();

					imshow("Sample", frame);

					quit = waitKey(100);

					/*auto start = std::chrono::high_resolution_clock::now();
					vCap.read(frame);
					auto finish = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = finish - start;
					cout << "Time taken to read frame = " << elapsed.count() << endl;

					ld.configParams.edgeParams.currImg = frame;
					start = std::chrono::high_resolution_clock::now();
					ld.processImg();
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for image processing = " << elapsed.count() << endl;
					start = std::chrono::high_resolution_clock::now();
					imshow("Sample", frame);
					finish = std::chrono::high_resolution_clock::now();
					elapsed = finish - start;
					cout << "Time taken for imshow = " << elapsed.count() << endl;
					waitKey(100);*/
				}


			}
			destroyWindow("Sample");
			return 1;
		}
	}

	else {
		Mat frame;
		VideoCapture vCap;
		vCap.open("../data/dashboardVid.mp4");
		bool firstFrame = false;

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

		else {
			//cout << "Number of frames = " << vCap.get(CAP_PROP_FRAME_COUNT) << endl;
			vCap.read(frame);
		}
	}
}
void setup(LineDetector& ld_, Mat& img) {

	int maxHeight = int(ld_.configParams.edgeParams.screenHeight / 2);
	int maxWidth = int(ld_.configParams.edgeParams.screenWidth / 3);

	int new_cols = int(float(maxHeight) / float(img.rows) * img.cols);
	int new_rows = int(float(maxWidth) / float(img.cols) * img.rows);

	if (new_cols > maxWidth) {
		new_cols = maxWidth;
	}

	else {
		new_rows = maxHeight;
	}

	ld_.configParams.edgeParams.newCols = new_cols;
	ld_.configParams.edgeParams.newRows = new_rows;
	ld_.configParams.edgeParams.recHeight = new_rows;
	ld_.configParams.edgeParams.recWidth = new_cols;

	cv::Mat m;
	const std::string winName = "Background";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	m.create(int(ld_.configParams.edgeParams.screenHeight*0.95), ld_.configParams.edgeParams.screenWidth, CV_32FC3);
	m.setTo(cv::Scalar(255, 255, 255));
	moveWindow(winName, 0, 0);
	cv::imshow(winName, m);

	ld_.configParams.edgeParams.origImg = img.clone();
	ld_.setROI_Box();
	//LineDetector::PrintFullPath(".\\");
	//LineDetector::func();
}

