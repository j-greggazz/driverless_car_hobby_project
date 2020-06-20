#include <lineDetector.h>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
//#define _USE_MATH_DEFINES


using namespace cv;
using namespace std;

void setup(LineDetector& ld_, Mat& img);
int loadVideo(bool multithreading = false);
void lineDetectionTest() {



	loadVideo();
	//Mat img = imread("../data/sample_car.jpg");
	//setup(ld, img);
	waitKey();

	//LineDetector::displayImg(ld.configParams.edgeParams.cannyImg, "FINISHED", ld.configParams.edgeParams.screenWidth, ld.configParams.edgeParams.screenHeight, -1);
	//cout << "here" << endl;
	//cout << "Canny settings: " << ld.configParams.edgeParams.highThresh << endl;
}

int loadVideo(bool multithreading) {

	LineDetector ld;
	if (!multithreading) {
		//--- INITIALIZE VIDEOCAPTURE
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
			const int num_threads = 3;

			vector<LineDetector> lineDetectors; // (num_threads, ld);
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
					//frameThreads[i] = thread(processImg_thread, std::ref(lineDetectors[i]), std::ref(cc), std::ref(quit));//, weldpoints_unedited); THIS IS NOT A FUNCTION CALL. THIS IS A CONSTRUCTOR - THEREFORE HAVE TO EXPLAIN WHICH ARGUMENTS ARE BOUND!
				}

				else if (i < 3) {
					vCap.read(frame);
					LineDetector ld = lineDetectors[i];
					ld.configParams.edgeParams.id = i;
					ld.configParams.edgeParams.currImg = frame;
					ld.configParams.edgeParams.newImgAvailable = true;
					ld.configParams.edgeParams.continueProcessing = true;
					frameThreads[i] = std::thread(LineDetector::processImg_thread, std::ref(ld));
					i++;
				}


				else if ((vCap.get(CAP_PROP_POS_FRAMES) + 1) < vCap.get(CAP_PROP_FRAME_COUNT)) {       // continue processing as long as further frames are available

					vCap.read(frame);

					if (i % num_threads == 0) {
						try {
							imshow("Sample0", lineDetectors[0].configParams.edgeParams.currImg.clone());
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
						cout << "%3 == 0, frame " << i << endl;
						lineDetectors[0].configParams.edgeParams.currImg = frame;
						lineDetectors[0].configParams.edgeParams.newImgAvailable = true;
					}


					if (i % num_threads == 1) {
						try {
							imshow("Sample1", lineDetectors[1].configParams.edgeParams.currImg.clone());
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
						cout << "%3 == 1, frame " << i << endl;
						lineDetectors[1].configParams.edgeParams.currImg = frame;
						lineDetectors[1].configParams.edgeParams.newImgAvailable = true;
					}

					if (i % num_threads == 2) {
						try {
							imshow("Sample2", lineDetectors[2].configParams.edgeParams.currImg.clone());
						}
						catch (cv::Exception& e) {
							int val = 0;
						}
						cout << "%3 == 2, frame " << i << endl;
						lineDetectors[2].configParams.edgeParams.currImg = frame;
						lineDetectors[2].configParams.edgeParams.newImgAvailable = true;
					}

					i++;






					//ld.configParams.edgeParams.currImg = frame;

					//ld.processImg();

					//imshow("Sample", frame);

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
			if (quit != 'q') {
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

